import sys
import socket
from urllib.parse import urlparse
import json
import pickle
import copy

import numpy as np
import torch
from torch.autograd import Variable
import argparse
import math
import os
from datetime import datetime

from lib import ml_modules

class MJAI_Client():
        def __init__(self):
                self.uri = urlparse(sys.argv[1])
                print(self.uri)
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                print(self.uri.hostname)
                print(self.uri.port)

                self.socket.connect((self.uri.hostname, self.uri.port))
                self.scores = [25000, 25000, 25000, 25000]
                self.reach = False
                self.tehais = []
                self.legal_actions = []
                self.current_record = []
                self.ai = None
        
        def run(self):
                while True:
                        self.msg = self.socket.recv(1024).decode("utf-8")
                        print("<-\t" + self.msg)
                        self.action = json.loads(self.msg)
                        if self.action["type"] == "hello":
                                self.response = {"type":"join", "name":"SL_AI", "room":self.uri.path[1:]}
                        elif self.action["type"] == "start_game":
                                self.current_record.append(self.action)
                                self.id = self.action["id"]
                                self.ai = ml_modules.Supervised_AI(self.id, "SL")
                                self.response = {"type":"none"}
                        elif self.action["type"] == "start_kyoku":
                                self.step_num = 0
                                self.a = copy.deepcopy(self.action)
                                self.a["scores"] = self.scores
                                self.current_record.append(self.a)
                                self.tehais = self.action["tehais"][self.id]
                                self.episode = []
                                self.response = {"type":"none"}
                        elif self.action["type"] == "hora" or self.action["type"] == "ryukyoku":
                                self.reach = False
                                self.scores = self.action["scores"]
                                self.current_record.append(self.action)
                                self.response = {"type":"none"}
                        elif self.action["type"] == "end_game":
                                my_score = self.action["scores"][self.id]
                                self.action["scores"].sort(reverse=True)
                                self.my_rank = self.action["scores"].index(my_score) + 1
                                f = open('rank.txt','a')
                                f.write(str(self.my_rank) + '\n')
                                f.close()
                                break
                        elif self.action["type"] == "error":
                                break
                        elif self.action["type"] == "tsumo" and self.action["actor"] == self.id:#dahai just after tsumo
                                self.current_record.append(self.action)
                                self.prev_tsumo = self.action["pai"]
                                self.tehais.append(self.action["pai"])
                                self.legal_actions = self.action["possible_actions"]
                                if self.reach:#after reach
                                        self.legal_actions.append({"type":"dahai", "actor":self.id, "pai":self.action["pai"], "tsumogiri":True})
                                else:
                                        for i in range(len(self.tehais)):
                                                self.legal_actions.append({"type":"dahai", "actor":self.id, "pai":self.tehais[i], "tsumogiri":i == len(self.tehais)-1})

                                self.act = self.ai.choose_action(self.current_record, self.legal_actions)

                                if self.act["type"] == "dahai" and (not self.reach):
                                        if self.act["pai"] == "5mr" and "5m" in self.tehais:
                                                self.act["pai"] = "5m"
                                                self.act["tsumogiri"] = True if self.prev_tsumo == "5m" else False
                                        if self.act["pai"] == "5pr" and "5p" in self.tehais:
                                                self.act["pai"] = "5p"
                                                self.act["tsumogiri"] = True if self.prev_tsumo == "5p" else False
                                        if self.act["pai"] == "5sr" and "5s" in self.tehais:
                                                self.act["pai"] = "5s"
                                                self.act["tsumogiri"] = True if self.prev_tsumo == "5s" else False        
                
                                self.response = self.act
                                self.legal_actions = []
                        elif (self.action["type"] == "chi" or self.action["type"] == "pon") and self.action["actor"] == self.id:#dahai just after fuuro
                                self.current_record.append(self.action)
                                for hai in self.action["consumed"]:
                                        self.tehais.remove(hai)
                                self.can_dahai = copy.deepcopy(self.tehais)
                                for hai in self.action["cannot_dahai"]:
                                        self.can_dahai = [i for i in self.can_dahai if i != hai]
                                for hai in self.can_dahai:
                                        self.legal_actions.append({"type":"dahai", "actor":self.id, "pai":hai, "tsumogiri":False})

                                self.act = self.ai.choose_action(self.current_record, self.legal_actions)
                                        
                                if self.act["pai"] == "5mr" and "5m" in self.tehais:
                                        self.act["pai"] = "5m"
                                if self.act["pai"] == "5pr" and "5p" in self.tehais:
                                        self.act["pai"] = "5p"
                                if self.act["pai"] == "5sr" and "5s" in self.tehais:
                                        self.act["pai"] = "5s"
                
                                self.response = self.act
                                self.legal_actions = []

                        elif self.action["type"] == "reach" and self.action["actor"] == self.id:#dahai just after reach
                                self.reach = True
                                self.current_record.append(self.action)
                                self.can_dahai = copy.deepcopy(self.tehais)
                                for hai in self.action["cannot_dahai"]:
                                        self.can_dahai = [i for i in self.can_dahai if i != hai]
                                for hai in self.can_dahai:
                                        self.legal_actions.append({"type":"dahai", "actor":self.id, "pai":hai, "tsumogiri":hai == self.prev_tsumo})
                                                
                                self.act = self.ai.choose_action(self.current_record, self.legal_actions)

                                if self.act["pai"] == "5mr" and "5m" in self.tehais:
                                        self.act["pai"] = "5m"
                                        self.act["tsumogiri"] = True if self.prev_tsumo == "5m" else False
                                if self.act["pai"] == "5pr" and "5p" in self.tehais:
                                        self.act["pai"] = "5p"
                                        self.act["tsumogiri"] = True if self.prev_tsumo == "5p" else False
                                if self.act["pai"] == "5sr" and "5s" in self.tehais:
                                        self.act["pai"] = "5s"
                                        self.act["tsumogiri"] = True if self.prev_tsumo == "5s" else False
                
                                self.response = self.act
                                self.legal_actions = []

                        elif (self.action["type"] == "dahai" or self.action["type"] == "kakan") and self.action["actor"] == self.id: #---tehai change---
                                self.current_record.append(self.action)
                                self.tehais.remove(self.action["pai"])
                                self.response = {"type":"none"}
                        elif (self.action["type"] == "daiminkan" or self.action["type"] == "ankan") and self.action["actor"] == self.id:
                                self.current_record.append(self.action)
                                for hai in self.action["consumed"]:
                                        self.tehais.remove(hai)
                                self.response = {"type":"none"}

                        elif self.action["type"] == "dahai" and self.action["actor"] != self.id:#fuuro
                                self.current_record.append(self.action)
                                if len(self.action["possible_actions"]) > 0:
                                        self.legal_actions = self.action["possible_actions"]
                                        self.response = self.ai.choose_action(self.current_record, self.legal_actions)
                                        self.legal_actions = []
                                else:
                                        self.response = {"type":"none"}
                        else:
                                self.current_record.append(self.action)
                                self.response = {"type":"none"}
                        #print("rec: ", current_record)
                        print("->\t" + json.dumps(self.response))
                        self.socket.sendall(bytes(json.dumps(self.response)+"\n", "utf-8"))

if __name__ == "__main__":
        client = MJAI_Client()
        client.run()