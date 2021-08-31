#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <array>
#include <algorithm>
#include <vector>
#include <cmath>
#define SCORE_NUM 15
#define SHANTEN_LIMIT 3

using mentsu_t = std::array<int, 3>;

int han_fu_score(int han, int fu, bool is_oya) {
  if (han == 0) return 0;
  if (fu % 10 != 0 && fu != 25) fu = fu + 10 - (fu % 10);
  int base_score = fu * std::pow(2, han+2);
  int score = 0;
  if (base_score >= 2000) {
    if (han <= 5) base_score = 2000;
    else if(han == 6 || han == 7) base_score = 3000;
    else if(han == 8 || han == 9 || han == 10) base_score = 4000;
    else if(han == 11 || han == 12) base_score = 6000;
    else base_score = 8000;
  }
  if (is_oya) score = base_score * 6;
  else score = base_score * 4;
  if (score < 1000) score = 1000;
  if (score % 100 != 0) score = score + 100 - (score % 100);
  return score;
}

bool is_shuntsu(std::array<int, 3> &mentsu) {
  return mentsu[0] != mentsu[1];
}

mentsu_t mid_to_mentsu(int mid) {
  mentsu_t mentsu;
  if (mid < 21) {
    for (int i=0; i<3; ++i) mentsu[i] = (mid / 7) * 9 + (mid % 7) + i;
  }
  else {
    for (int i=0; i<3; ++i) mentsu[i] = mid - 21;
  }
  return mentsu;
}

int mentsu_to_mid(mentsu_t &mentsu) {
  if (is_shuntsu(mentsu)) return (mentsu[0] / 9) * 7 + (mentsu[0] % 9);
  else return mentsu[0] + 21; 
}

std::array<int, 34> te_array(int atama, std::array<int, 4> &mentsu) {
  std::array<int, 34> te{};
  te[atama] += 2;
  for (int i=0; i<4; i++) {
    mentsu_t m = mid_to_mentsu(mentsu[i]);
    for (int j=0; j<3; j++) te[m[j]] ++;
  }
  return te;
}

std::array<int, 34> furo_to_hai_array(std::array<int, 4> &furo, int furo_num){
  std::array<int, 34> hai_array{};
  for (int i=0; i<furo_num; i++) {
    mentsu_t mentsu = mid_to_mentsu(furo[i]);
    for (int j=0; j<3; ++j) hai_array[mentsu[j]] ++;
  }
  return hai_array;
}

class Hand
{
  std::array<int, 34> m_te;
public:
  Hand() { m_te.fill(0); }
  Hand(std::array<int, 34> &tehai) : m_te(tehai) {}
  int hai_num(int hai) const noexcept { return m_te[hai]; }
  void inc_toitsu(int hai) noexcept;
  void dec_toitsu(int hai);
  void inc_shuntsu(int hai);
  void dec_shuntsu(int hai);
  void inc_kotsu(int hai);
  void dec_kotsu(int hai) noexcept;
  int chitoi_shanten_num();
  int yaochu_toitsu_num();
  int kokushi_shanten_num();
  void print();
};
 
void Hand::inc_toitsu(int hai) noexcept{
  m_te[hai] += 2;
}

void Hand::dec_toitsu(int hai){
  if (m_te[hai] > 2) m_te[hai] -= 2;
  else m_te[hai] = 0;
}

void Hand::inc_shuntsu(int hai){
  m_te[hai] ++;
  m_te[hai+1] ++;
  m_te[hai+2] ++;
}

void Hand::dec_shuntsu(int hai){
  for (int i=0; i<3; ++i) {
    if (m_te[hai+i] > 0) m_te[hai+i] -= 1;
    else m_te[hai+i] = 0;
  }
}

void Hand::inc_kotsu(int hai){
  m_te[hai] += 3;
}

void Hand::dec_kotsu(int hai) noexcept {
  if (m_te[hai] > 3) m_te[hai] -= 3;
  else m_te[hai] = 0;
}

int Hand::chitoi_shanten_num() {
  int toitsu_num = 0;
  int hai_kind_num = 0;
  for (int i=0; i<34; i++) {
    if (m_te[i] > 0) hai_kind_num ++;
    if (m_te[i] > 1) toitsu_num ++;
  }
  if (hai_kind_num >= 7) return 6 - toitsu_num;
  else return 13 - toitsu_num - hai_kind_num;
}

int Hand::yaochu_toitsu_num() {
  int y = 0;
  for (int i=0; i<34; i++) {
    if ((i > 26 || (i < 27 && (i%9 == 0 || i%9 == 8))) && m_te[i] > 1 ) y ++; 
  }
  return y;
}

int Hand::kokushi_shanten_num() {
  int yaochu_kind_num = 0;
  for (int i=0; i<34; i++) {
    if ((i > 26 || (i < 27 && (i%9 == 0 || i%9 == 8))) && m_te[i] > 0) yaochu_kind_num ++;
  }
  if (yaochu_toitsu_num() > 0) return 13 - yaochu_kind_num - 1;
  else return 13 - yaochu_kind_num;
}

void Hand::print(){
  for(int i=0; i<34; ++i) printf("%d", m_te[i]);
  printf("\n");
}

class CPass
{
  int m_toitsu_num;
  std::array<int, 7> m_toitsu;

public:
  CPass() : m_toitsu_num(0) 
  {
    m_toitsu.fill(-1);
  }
  CPass(int toitsu_num, std::array<int, 7> &toitsu) : m_toitsu_num(toitsu_num),  m_toitsu(toitsu) {}
  bool is_terminal_node() { return m_toitsu_num == 7; }
  void add(int hai) { m_toitsu[m_toitsu_num] = hai;  m_toitsu_num ++; } 
  void print();
  int score(bool is_reach, bool is_tsumo, std::array<int, 34> &nokori_array, std::array<int, 5> &dora, int aka_num, bool is_oya, std::vector<std::string> &yaku);
};

void CPass::print() {
  for (int i=0; i<m_toitsu_num; i++) {
    printf("[%d,%d] ", m_toitsu[i], m_toitsu[i]);
  }
  //printf("\n");
}

int CPass::score(bool is_reach, bool is_tsumo, std::array<int, 34> &nokori_array, std::array<int, 5> &dora, int aka_num, bool is_oya, std::vector<std::string> &yaku) {
  int han = 2;
  int fu = 25;

  int over_hai_num = 0;
  for (int i=0; i<7; i++) {
    if (nokori_array[m_toitsu[i]] < 2) over_hai_num += 2 - nokori_array[m_toitsu[i]];
  }
  if (over_hai_num == 1) return 0;//keiten
  if (over_hai_num > 1) return -1;//cannot keiten

  if (is_reach) { han ++; yaku.push_back("reach");}//reach
  if (is_tsumo) { han ++; yaku.push_back("tsumo");}//tsumo
  bool is_tanyao = true;
  int roto_num = 0;
  int jihai_num = 0;
  int somete_color = -1;// -2:not somete. -1:init value. 0:manzu. 1:pinzu. 2:sozu.
  int dora_num = 0;
  for (int i=0; i<7; i++) {
    for (int d=0; dora[d] >= 0; d++) {
      if (m_toitsu[i] == dora[d]) { dora_num += 2; }
    }
    if (m_toitsu[i] > 26) {
      is_tanyao = false;
      jihai_num ++;
    }
    if (m_toitsu[i] < 27 && (m_toitsu[i]%9 == 0 || m_toitsu[i]%9 == 8)) {
      is_tanyao = false;
      roto_num ++;
    }
    if (somete_color == -1 && m_toitsu[i] < 27) somete_color = m_toitsu[i]/9;
    if (somete_color > 0 && somete_color != m_toitsu[i]/9) somete_color = -2;
  }

  if (is_tanyao) { han ++; yaku.push_back("tanyao");}//tanyao
  yaku.push_back("chitoi");//chitoi
  if (jihai_num == 7) { han = 13; yaku.push_back("tsuiso");}//tuiso
  else if(roto_num + jihai_num == 7) { han += 2; yaku.push_back("honroto");}//honroto
  if (somete_color > 0 && jihai_num == 0) { han += 6; yaku.push_back("chinitsu");}//chinitsu
  else if (somete_color > 0 && jihai_num > 0) { han += 3; yaku.push_back("honitsu");}//honitsu
  if (dora_num > 0) { han += dora_num; yaku.push_back("dora:" + std::to_string(dora_num)); }//dora
  if (aka_num > 0) {
    int num_5 = 0;
    for (int i=0; i<3; i++) {
      if (m_toitsu[i*9+4] > 0) num_5 ++;
    }
    aka_num = std::min(aka_num,num_5);
    han += aka_num; 
    yaku.push_back("aka:" + std::to_string(aka_num)); 
  }//aka

  return han_fu_score(han, fu, is_oya); 
}

class Pass
{
  int m_toitsu;
  int m_mentsu_num;
  std::array<int, 4> m_mentsu;
  int m_furo_num;
  std::array<int, 4> m_furo;

public:
  Pass() : m_toitsu(-1), m_mentsu_num(0), m_furo_num(0) 
  {
    m_mentsu.fill(-1);
    m_furo.fill(-1);
  }
  Pass(int furo_num, std::array<int, 4> &furo) : m_toitsu(-1), m_mentsu_num(furo_num), m_mentsu(furo), m_furo_num(furo_num), m_furo(furo) {}
  int furo_num() { return m_furo_num; }
  bool is_terminal_node() { return m_toitsu >= 0 && m_mentsu_num == 4; }
  bool has_toitsu() { return m_toitsu >= 0; }
  void add_toitsu(int hai) { m_toitsu = hai;} 
  void add_mentsu(int mid);
  void print();
  int score(bool is_reach, bool is_tsumo, std::array<int, 34> &nokori_array, std::array<int, 5> &dora, int jikaze, int bakaze, int ankan_num, int aka_num, bool is_oya, std::vector<std::string> &yaku);
};

void Pass::add_mentsu(int mid) {
  m_mentsu[m_mentsu_num] = mid;
  m_mentsu_num ++;
}
void Pass::print() {
  printf("[%d,%d] ", m_toitsu, m_toitsu);
  for (int i=0; i<m_mentsu_num; i++) {
    std::array<int, 3> mentsu = mid_to_mentsu(m_mentsu[i]);
    printf("[%d,%d,%d] ", mentsu[0], mentsu[1], mentsu[2]);
  }
  //printf("\n");
}

int Pass::score(bool is_reach, bool is_tsumo, std::array<int, 34> &nokori_array, std::array<int, 5> &dora, int jikaze, int bakaze, int ankan_num, int aka_num, bool is_oya, std::vector<std::string> &yaku) {
  //kaze  0:東 1:南 2:西 3:北
  int han = 0;
  int fu = 20;
  bool is_pinfu = false;
  std::array<int, 34> te = te_array(m_toitsu, m_mentsu);
  
  int over_hai_num = 0;
  for (int i=0; i<34; i++) {
    if (te[i] > nokori_array[i]) over_hai_num += nokori_array[i] - te[i];
  }
  if (over_hai_num == 1) return 0;//keiten
  if (over_hai_num > 1) return -1;//cannot keiten

  if (m_furo_num == 0){//menzen_yaku

    if (is_reach) { han ++; yaku.push_back("reach"); }//reach
    if (is_tsumo) { han ++; yaku.push_back("tsumo"); }//tsumo

    bool is_all_shuntsu = true;
    for (int i=0; i<4; ++i){
      if (m_mentsu[i] >= 21) is_all_shuntsu = false;
    } 
    if (m_toitsu < 31 && m_toitsu != 27+jikaze && m_toitsu != 27+bakaze && is_all_shuntsu) is_pinfu = true;
    if (is_pinfu) { han ++; yaku.push_back("pinfu"); }//pinfu without ryanmen_machi                                                                                                                    
    int same_shuntsu_num = 0;
    int same_shuntsu_index0 = -1;
    int same_shuntsu_index1 = -1;
    for (int i=0; i<3; ++i) {                                                                        
      for (int j=i+1; j<4; ++j) {
	if (i != same_shuntsu_index0 && j != same_shuntsu_index1 && m_mentsu[i] == m_mentsu[j] && m_mentsu[i] < 21){
	  same_shuntsu_num ++;
	  if (same_shuntsu_index0 != -1) break;
	  same_shuntsu_index0 = i;
	  same_shuntsu_index1 = j;
	}
      }
    }
    if (same_shuntsu_num == 1) { han ++; yaku.push_back("ipeko"); }//ipeko
    else if (same_shuntsu_num == 2) { han += 3; yaku.push_back("ryanpeko"); }//ryanpeko
  }//menzen_yaku

  bool is_tanyao = true;
  std::array<int, 13> yaochu_idx = {0,8,9,17,18,26,27,28,29,30,31,32,33};
  for (int i=0; i<13; ++i){
    if (te[yaochu_idx[i]] > 0){
      is_tanyao = false; 
      break;
    }
  }
  if (is_tanyao) { han ++; yaku.push_back("tanyao"); }//tanyao
  
  for (int i=0; i<4; ++i) {
    if (m_mentsu[i] == 48+jikaze) { han ++; yaku.push_back("jikaze"); }//jikaze
    if (m_mentsu[i] == 48+bakaze) { han ++; yaku.push_back("bakaze"); }//bakaze
    if (m_mentsu[i] > 51) { han ++; yaku.push_back("yakuhai"); }//yakuhai
  }
  
  std::array<std::array<int, 3>, 4> check_idx {{ {0,1,2}, {0,1,3}, {0,2,3}, {1,2,3} }};
  for (int i=0; i<4; ++i) {//sanshoku
    if (m_mentsu[check_idx[i][0]] < 21 && m_mentsu[check_idx[i][1]] < 21 && m_mentsu[check_idx[i][2]] < 21 && 
	m_mentsu[check_idx[i][0]]%7 == m_mentsu[check_idx[i][1]]%7 && 
	m_mentsu[check_idx[i][1]]%7 == m_mentsu[check_idx[i][2]]%7 &&
	m_mentsu[check_idx[i][0]] != m_mentsu[check_idx[i][1]] && 
	m_mentsu[check_idx[i][0]] != m_mentsu[check_idx[i][2]] && 
	m_mentsu[check_idx[i][1]] != m_mentsu[check_idx[i][2]]) 
      { //dojun 
	if (m_furo_num == 0) han += 2;
	else han ++;
	yaku.push_back("sanshoku");
	break;
      }
    if (m_mentsu[check_idx[i][0]] > 20 && m_mentsu[check_idx[i][1]] >20 && m_mentsu[check_idx[i][2]] > 20 && 
	m_mentsu[check_idx[i][0]] < 48 && m_mentsu[check_idx[i][1]] < 48 && m_mentsu[check_idx[i][2]] < 48 &&
	m_mentsu[check_idx[i][0]]%9 == m_mentsu[check_idx[i][1]]%9 &&
	m_mentsu[check_idx[i][1]]%9 == m_mentsu[check_idx[i][2]]%9 &&
	m_mentsu[check_idx[i][0]] != m_mentsu[check_idx[i][1]] && 
	m_mentsu[check_idx[i][0]] != m_mentsu[check_idx[i][2]] && 
	m_mentsu[check_idx[i][1]] != m_mentsu[check_idx[i][2]]) 
      { //doko
	han += 2; 
	yaku.push_back("sanshokudoko");
	break;
      }
  }
  int kotsu_num = 0;
  int anko_num = ankan_num;

  for (int i=0; i<4; ++i) {
    int fu_i = 0;
    if (m_mentsu[i] >= 21) {
      kotsu_num ++; fu_i = 2; 
      if (i >= m_furo_num && (i != 3 || is_tsumo)) { 
	anko_num ++; fu_i *= 2; 
      }
    }
    if (m_mentsu[i] > 47 || m_mentsu[i]%9 == 2 || m_mentsu[i]%9 == 3) fu_i *= 2;
    fu += fu_i;
  }
  if (kotsu_num == 4) { han += 2; yaku.push_back("toitoi"); }//toitoi
  if (anko_num == 3) { han += 2; yaku.push_back("sananko"); }//sananko
  if (anko_num == 4) { han = 13; yaku.push_back("suanko"); }//suanko

  int junchan_num = 0;
  int roto_num = 0; 
  int jihai_num = 0;
  if (m_toitsu >= 27) jihai_num ++;
  else if (m_toitsu % 9 == 0 || m_toitsu % 9 == 8) junchan_num ++; roto_num ++;
  for (int i=0; i<4; ++i) {
    if (m_mentsu[i] > 47) jihai_num ++;
    else if (m_mentsu[i] > 20 && (m_mentsu[i]%9 == 3 || m_mentsu[i]%9 == 2)) { roto_num ++; junchan_num ++; }
    if (m_mentsu[i] < 21 && (m_mentsu[i]%7 == 0 || m_mentsu[i]%7 == 6)) junchan_num ++;
  }
  if (roto_num == 5) { han = 13; yaku.push_back("chinroto"); }//chinroto
  else if(roto_num + jihai_num == 5) { han += 2; yaku.push_back("honroto"); }//honroto                                             
  else if (junchan_num == 5) {//junchan
    if (m_furo_num == 0) han += 3;
    else han += 2;
    yaku.push_back("junchan");
  }
  else if(junchan_num + jihai_num == 5) {//chanta
    if (m_furo_num == 0) han += 2;
    else han ++;
    yaku.push_back("chanta");
  }
  if (jihai_num == 5) { han = 13; yaku.push_back("tsuiso"); }//tuiso

  int sangen_mentsu_num = 0;
  for (int i=0; i<4; ++i) {
    if (m_mentsu[i] > 51) sangen_mentsu_num ++;
  }
  if (m_toitsu > 30 && sangen_mentsu_num == 2) { han += 2; yaku.push_back("shosangen"); }//shosangen
  if (sangen_mentsu_num == 3) { han = 13; yaku.push_back("daisangen"); }//daisangen

  int somete_color = -1;// -2:not somete. -1:init value. 0:manzu. 1:pinzu. 2:sozu.
  if (m_toitsu < 27) somete_color = m_toitsu / 9;
  for (int i=0; i<4; ++i) {
    if (m_mentsu[i] > 47) continue;//when jihai
    else { 
      int color;
      if (m_mentsu[i] >= 21) color = (m_mentsu[i] - 21) / 9;
      else color = m_mentsu[i] / 7;
      if (somete_color == -1) somete_color = color;
      else if (somete_color != color) { 
	somete_color = -2;
	break;
      }
    }
  }
  if (somete_color >= 0 && jihai_num == 0){//chinitsu
    if (m_furo_num == 0) han += 6;
    else han += 5;
    yaku.push_back("chinitsu");
    bool is_churen = true;
    for (int i=0; i<9; ++i) {
      if (i == 0 || i == 9) {
	if (te[9 * somete_color + i] < 3){
	  is_churen = false;
	  break;
	}
      }
      else if(te[9 * somete_color + i] == 0 || te[9 * somete_color + i] > 2){
	is_churen = false;
	break;
      }
    }
    if (is_churen && m_furo_num == 0) { han = 13; yaku.push_back("churen"); }//churen
  }
  if (somete_color >= 0 && jihai_num > 0) {// honitsu                                                                            
    if (m_furo_num == 0) han += 3;
    else han +=2;
    yaku.push_back("honitsu");
    if (somete_color == 2) {
      bool is_ryuiso = true;
      std::array<int, 4> not_ryuiso_idx = {18, 22, 24, 26};
      for (int i=0; i<4; i++) {
	if (te[not_ryuiso_idx[i]] > 0) {
	  is_ryuiso = false;
	  break;
	}
      }
      if (is_ryuiso) { han = 13; yaku.push_back("ryuiso"); }//ryuiso
    }
  }

  int dora_num = 0;
  if (han > 0) {
    for (int d=0; dora[d] >= 0; d++) dora_num += te[dora[d]];
    if (dora_num > 0) { han += dora_num; yaku.push_back("dora:" + std::to_string(dora_num)); }//dora
    if (aka_num > 0) {
      int num_5 = 0;
      for (int i=0; i<3; i++) {
	if (te[i*9+4] > 0) num_5 ++;
      }
      aka_num = std::min(aka_num,num_5);
      han += aka_num; 
      yaku.push_back("aka:" + std::to_string(aka_num)); }//aka_dora
  }

  // fu calc
  if (m_toitsu == 27+jikaze) fu += 2;
  if (m_toitsu == 27+bakaze) fu += 2;
  if (m_toitsu > 30) fu += 2;
  if (m_furo_num == 0 && !is_tsumo) fu += 10;
  if (is_tsumo) fu += 2;
  
  if (is_tsumo && is_pinfu) fu = 20;
  //

  return han_fu_score(han, fu, is_oya);
}
// member function finished

//bakyo
std::array<int, 34> nokori_array;
std::array<int, 5> dora;
bool is_reach;
bool is_tsumo; 
int jikaze; 
int bakaze;
int ankan_num;
int aka_num; 
bool is_oya;
int honba;
int kyotaku;
//

bool chitoi_dfs0(Hand &tehai, Hand &hand, CPass pass,int start_tid, int shanten_num, int &limit_shanten_num, std::array<std::array<int, 34>, 2*SCORE_NUM*(SHANTEN_LIMIT+1)> &feature){
  if (shanten_num > limit_shanten_num) return false;

  if (pass.is_terminal_node()) {
    //pass.print();
    std::vector<std::string> yaku;
    //int score = pass.score(is_reach, is_tsumo, dora, aka_num, is_oya, yaku);
    std::array<int, 4> scores;
    scores[0] = pass.score(true, true, nokori_array, dora, aka_num, is_oya, yaku);
    scores[1] = pass.score(true, false, nokori_array, dora, aka_num, is_oya, yaku);
    scores[2] = pass.score(false, true, nokori_array, dora, aka_num, is_oya, yaku);
    scores[3] = pass.score(false, false, nokori_array, dora, aka_num, is_oya, yaku);
    for (int i=0; i<4; i++) {
      int score = scores[i];
      if (score < 0) continue;//cannot hora
      if (score != 0) score += honba * 300 + kyotaku * 1000;
      //printf("%d %d", shanten_num, score);
      //for (int i=0; i<yaku.size(); i++) printf(" %s",yaku[i].c_str());
      //printf("\n");
      if (shanten_num >= 0) {
	int score_idx;
	if (score < 10000) score_idx = score / 1000 ;
	else if (score < 16000) score_idx = 10;
	else if (score < 18000) score_idx = 11;
	else if (score < 24000) score_idx = 12;
	else if (score < 32000) score_idx = 13;
	else score_idx = 14;
	for (int hai=0; hai<34; hai++) {
	  if (tehai.hai_num(hai) > 0) feature[(i / 2)*SCORE_NUM*(SHANTEN_LIMIT+1) + shanten_num*SCORE_NUM + score_idx][hai] = 1;
	}
      }
    }
    if (shanten_num < SHANTEN_LIMIT - 1 && limit_shanten_num == SHANTEN_LIMIT) limit_shanten_num = SHANTEN_LIMIT - 1;
    return true;
  }

  for (int i=start_tid; i<34; i++) {
    if (tehai.hai_num(i) == 1 && hand.hai_num(i) == 0) {
      Hand tehai_amari = tehai;
      Hand child_hand = hand;
      CPass new_pass = pass;
      int delta_shanten_num = 2 - tehai_amari.hai_num(i);
      tehai_amari.dec_toitsu(i);
      child_hand.inc_toitsu(i);
      new_pass.add(i);
      bool flag = chitoi_dfs0(tehai_amari, child_hand, new_pass, i, shanten_num+delta_shanten_num, limit_shanten_num, feature);
    }
  }
  return true;
}

bool agari_dfs0(Hand &tehai, Hand &hand, Pass &pass, int start_mid, int shanten_num, int &limit_shanten_num, std::array<std::array<int, 34>, 2*SCORE_NUM*(SHANTEN_LIMIT+1)> &feature) noexcept {
  //printf("hand: ");
  //hand.print();
  //printf("amari: ");
  //tehai.print();
  if (shanten_num > limit_shanten_num) return false;

  if (pass.is_terminal_node()) {
    //pass.print();
    std::vector<std::string> yaku;
    //int score = pass.score(is_reach, is_tsumo, dora, jikaze, bakaze, ankan_num, aka_num, is_oya, yaku);
    std::array<int, 4> scores;
    if (pass.furo_num() != 0) {scores[0] = -1; scores[1] = -1;}
    else {
      scores[0] = pass.score(true, true, nokori_array, dora, jikaze, bakaze, ankan_num, aka_num, is_oya, yaku);
      scores[1] = pass.score(true, false, nokori_array, dora, jikaze, bakaze, ankan_num, aka_num, is_oya, yaku);
    }
    scores[2] = pass.score(false, true, nokori_array, dora, jikaze, bakaze, ankan_num, aka_num, is_oya, yaku);
    scores[3] = pass.score(false, false, nokori_array, dora, jikaze, bakaze, ankan_num, aka_num, is_oya, yaku);
    for (int i=0; i<4; i++) {
      int score = scores[i];
      if (score < 0) continue;//cannot hora
      if (score != 0) score += honba * 300 + kyotaku * 1000;
      //printf("%d %d", shanten_num, score);
      //for (int i=0; i<yaku.size(); i++) printf(" %s",yaku[i].c_str());
      //printf("\n");
      if (shanten_num >= 0) {
	int score_idx;
	if (score < 10000) score_idx = score / 1000;
	else if (score < 16000) score_idx = 10;
	else if (score < 18000) score_idx = 11;
	else if (score < 24000) score_idx = 12;
	else if (score < 32000) score_idx = 13;
	else score_idx = 14;
	for (int hai=0; hai<34; hai++) {
	  if (tehai.hai_num(hai) > 0) feature[(i / 2)*SCORE_NUM*(SHANTEN_LIMIT+1) + shanten_num*SCORE_NUM + score_idx][hai] = 1;
	}
      }
    } 
    if (shanten_num < SHANTEN_LIMIT - 1 && limit_shanten_num == SHANTEN_LIMIT) limit_shanten_num = SHANTEN_LIMIT - 1;
    return true;
  }
  
  if (!pass.has_toitsu()) {
    for (int i=0; i<34; ++i) {
      Hand tehai_amari = tehai;
      Hand child_hand = hand;
      Pass new_pass = pass;
      int delta_shanten_num = 0;
      if (tehai_amari.hai_num(i) < 2) delta_shanten_num = 2 - tehai_amari.hai_num(i);
      tehai_amari.dec_toitsu(i);
      child_hand.inc_toitsu(i);
      new_pass.add_toitsu(i);
      bool flag = agari_dfs0(tehai_amari, child_hand, new_pass, 0, shanten_num+delta_shanten_num, limit_shanten_num, feature);
    }
    return true;
  }

  for(int i=start_mid; i<55; ++i){
    mentsu_t mentsu = mid_to_mentsu(i); 
    if(i < 21 && hand.hai_num(mentsu[0]) < 4 && hand.hai_num(mentsu[1]) < 4 && hand.hai_num(mentsu[2]) < 4){// make shuntsu
      Hand tehai_amari = tehai;
      Hand child_hand = hand;
      Pass new_pass = pass;
      int delta_shanten_num = 0;
      for(int j=0; j<3; ++j){
        if (tehai_amari.hai_num(mentsu[j]) < 1) delta_shanten_num ++ ;
      }
      tehai_amari.dec_shuntsu(mentsu[0]);
      child_hand.inc_shuntsu(mentsu[0]);
      new_pass.add_mentsu(i);
      bool flag = agari_dfs0(tehai_amari, child_hand, new_pass, i, shanten_num+delta_shanten_num, limit_shanten_num, feature);
    }
    if(i >= 21 && hand.hai_num(mentsu[0]) < 2){// make kotsu
      Hand tehai_amari = tehai;
      Hand child_hand = hand;
      Pass new_pass = pass;
      int delta_shanten_num = 0;
      if (tehai_amari.hai_num(mentsu[0]) < 3) delta_shanten_num = 3 - tehai_amari.hai_num(mentsu[0]);
      tehai_amari.dec_kotsu(mentsu[0]);
      child_hand.inc_kotsu(mentsu[0]);
      new_pass.add_mentsu(i);
      bool flag = agari_dfs0(tehai_amari, child_hand, new_pass, i, shanten_num+delta_shanten_num, limit_shanten_num, feature);
    }
  }
  return true;
}

void chitoi_dfs(Hand &tehai, int limit_shanten_num, std::array<std::array<int, 34>, 2*SCORE_NUM*(SHANTEN_LIMIT+1)> &feature) {
  int shanten_num = -1;
  int start_tid = 0;
  Hand hand;
  CPass pass;
  for (int i=0; i<34; i++) {
    if (tehai.hai_num(i) > 1) {
      tehai.dec_toitsu(i);
      hand.inc_toitsu(i);
      pass.add(i);
    }
  }
  chitoi_dfs0(tehai, hand, pass, start_tid, shanten_num, limit_shanten_num, feature);
}

void agari_dfs(Hand &tehai, Hand &hand, Pass &pass, int limit_shanten_num, std::array<std::array<int, 34>, 2*SCORE_NUM*(SHANTEN_LIMIT+1)> &feature) {
  int start_mid = 0;
  int shanten_num = -1;
  agari_dfs0(tehai, hand, pass, start_mid, shanten_num, limit_shanten_num, feature);
}

int main(int argc, char *argv[]){
  assert(argc == 13);
  //std::array<int, 34> te{2,2,2,0,0,0,0,0,1,0,2,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0};
  //std::array<int ,4> furo {-1, -1, -1, -1};
  //int furo_num = 0;
  
  std::array<int, 34> te{};
  for (int i=0; i<34; i++) {
    int t = argv[1][2*i] - '0';
    te[i] = t;  
  }
  
  std::array<int, 4> furo;
  int f_idx = 0;
  char *f = std::strtok(argv[2], ",");
  while (f) {
    int ival;
    ival = atoi(f);
    //std::cout << f_idx << "> " << ival << std::endl;
    furo[f_idx] = ival;
    f_idx += 1;
    f = std::strtok(nullptr, ",");
  }
  assert(f_idx == 4);
  int furo_num = atoi(argv[3]);
  Hand tehai(te);
  std::array<int, 34> furo_array = furo_to_hai_array(furo, furo_num);
  Hand hand(furo_array);
  Pass pass(furo_num, furo);

  std::array<std::array<int, 34>, 2*SCORE_NUM*(SHANTEN_LIMIT+1)> feature;
  for (int i=0; i<2*SCORE_NUM*(SHANTEN_LIMIT+1); i++) {
    feature[i].fill(0);
  }

  //bakyo
  is_reach = false;
  is_tsumo = false;
  //dora = {2,-1,-1,-1,-1}; 
  //jikaze = 0;
  //bakaze = 0;
  //ankan_num = 0;
  //aka_num = 0; 
  //is_oya = true;
  //honba = 0;
  //kyotaku = 0;
  int d_idx = 0;
  char *d = std::strtok(argv[4], ",");
  while (d) {
    int ival;
    ival = atoi(d);
    //std::cout << d_idx << "> " << ival << std::endl;
    dora[d_idx] = ival;
    d_idx += 1;
    d = std::strtok(nullptr, ",");
  }
  assert(d_idx == 5);
  jikaze = atoi(argv[5]);
  bakaze = atoi(argv[6]);
  ankan_num = atoi(argv[7]);
  aka_num = atoi(argv[8]);
  if (atoi(argv[9]) == 1) is_oya = true;
  else is_oya = false;
  honba = atoi(argv[10]);
  kyotaku = atoi(argv[11]);
  int n_idx = 0;
  char *n = std::strtok(argv[12], ",");
  while (n) {
    int ival;
    ival = atoi(n);
    //std::cout << d_idx << "> " << ival << std::endl;
    nokori_array[n_idx] = ival;
    n_idx += 1;
    n = std::strtok(nullptr, ",");
  }
  for (int i=0; i<furo_num; i++) {
    for (int j=0; j<3; j++) {
      nokori_array[mid_to_mentsu(furo[i])[0]] ++;
    }
  }
  //

  /*
  printf("tehai: ");
  tehai.print();
  printf("furo: ");
  //hand.print();
  for (int i=0; i<furo_num; ++i) {
    printf("[%d,%d,%d] ", mid_to_mentsu(furo[i])[0], mid_to_mentsu(furo[i])[1], mid_to_mentsu(furo[i])[2]);
  }
  printf("\n");
  printf("honba:%d kyotaku:%d\n", honba, kyotaku);
  */

  int limit_shanten_num = 2;
  if (furo_num == 0 && tehai.kokushi_shanten_num() <= limit_shanten_num) {//kokushi
    for (int i=0; i<34; i++) {
      if (i < 27 && i%9 != 0 && i%9 != 8 && tehai.hai_num(i) > 0) feature[SCORE_NUM*(SHANTEN_LIMIT+1) + tehai.kokushi_shanten_num()*SCORE_NUM + 14][i] = 1; 
      else if(tehai.yaochu_toitsu_num() > 1 && tehai.hai_num(i) > 1) feature[SCORE_NUM*(SHANTEN_LIMIT+1) + tehai.kokushi_shanten_num()*SCORE_NUM + 14][i] = 1;
    }
    //printf("kokushi_shanten_num: %d", tehai.kokushi_shanten_num());
  }
  if (furo_num == 0 && tehai.chitoi_shanten_num() <= limit_shanten_num) {//chitoi
    Hand tehai_copy = tehai;
    chitoi_dfs(tehai_copy, limit_shanten_num, feature);
  }
  agari_dfs(tehai, hand, pass, limit_shanten_num, feature);//1 janto 4 mentsu
  
  for (int i=0; i<2*SCORE_NUM*(SHANTEN_LIMIT+1); i++) {
    for (int j=0; j<34; j++) {
      printf("%d", feature[i][j]);
    }
    printf("\n");
  }
  
  return 0;
}
