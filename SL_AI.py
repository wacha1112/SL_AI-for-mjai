import sys
import socket
from urllib.parse import urlparse
import json
import pickle
import copy

import numpy as np
import argparse
import math

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
                self.current_record = []
                self.ai = None
                self.id = 0
                self.prev_tsumo = None
        
        def run(self):
                while True:
                        msg = self.socket.recv(1024).decode("utf-8")
                        print("<-\t" + msg)
                        action = json.loads(msg)
                        legal_actions = []
                        if action["type"] == "hello":
                                response = {"type":"join", "name":"SL_AI", "room":self.uri.path[1:]}
                        elif action["type"] == "start_game":
                                self.current_record.append(action)
                                self.id = action["id"]
                                self.ai = ml_modules.Supervised_AI(self.id, "SL")
                                response = {"type":"none"}
                        elif action["type"] == "start_kyoku":
                                a = copy.deepcopy(action)
                                a["scores"] = self.scores
                                self.current_record.append(a)
                                self.tehais = action["tehais"][self.id]
                                response = {"type":"none"}
                        elif action["type"] == "hora" or action["type"] == "ryukyoku":
                                self.reach = False
                                self.scores = action["scores"]
                                self.current_record.append(action)
                                response = {"type":"none"}
                        elif action["type"] == "end_game":
                                my_score = action["scores"][self.id]
                                action["scores"].sort(reverse=True)
                                my_rank = action["scores"].index(my_score) + 1
                                f = open('rank.txt','a')
                                f.write(str(my_rank) + '\n')
                                f.close()
                                break
                        elif action["type"] == "error":
                                break
                        elif action["type"] == "tsumo" and action["actor"] == self.id:#dahai just after tsumo
                                self.current_record.append(action)
                                self.prev_tsumo = action["pai"]
                                self.tehais.append(action["pai"])
                                legal_actions = action["possible_actions"]
                                if self.reach:#after reach
                                        legal_actions.append({"type":"dahai", "actor":self.id, "pai":action["pai"], "tsumogiri":True})
                                else:
                                        for i in range(len(self.tehais)):
                                                legal_actions.append({"type":"dahai", "actor":self.id, "pai":self.tehais[i], "tsumogiri":i == len(self.tehais)-1})

                                act = self.ai.choose_action(self.current_record, legal_actions)

                                if act["type"] == "dahai" and not self.reach:
                                        if act["pai"][-1] == "r" and act["pai"][:2] in self.tehais:
                                                act["pai"] = act["pai"][:2]
                                                act["tsumogiri"] = True if self.prev_tsumo == act["pai"] else False
                
                                response = act
                        elif (action["type"] == "chi" or action["type"] == "pon") and action["actor"] == self.id:#dahai just after fuuro
                                self.current_record.append(action)
                                for hai in action["consumed"]:
                                        self.tehais.remove(hai)
                                can_dahai = copy.deepcopy(self.tehais)
                                for hai in action["cannot_dahai"]:
                                        can_dahai = [i for i in can_dahai if i != hai]
                                for hai in can_dahai:
                                        self.legal_actions.append({"type":"dahai", "actor":self.id, "pai":hai, "tsumogiri":False})

                                act = self.ai.choose_action(self.current_record, legal_actions)
                                        
                                if act["pai"][-1] == "r" and act["pai"][:2] in self.tehais:
                                        act["pai"] = act["pai"][:2]
                                        act["tsumogiri"] = True if self.prev_tsumo == act["pai"] else False
                
                                response = act

                        elif action["type"] == "reach" and action["actor"] == self.id:#dahai just after reach
                                self.reach = True
                                self.current_record.append(action)
                                can_dahai = copy.deepcopy(self.tehais)
                                for hai in action["cannot_dahai"]:
                                        can_dahai = [i for i in can_dahai if i != hai]
                                for hai in can_dahai:
                                        legal_actions.append({"type":"dahai", "actor":self.id, "pai":hai, "tsumogiri":hai == self.prev_tsumo})
                                                
                                act = self.ai.choose_action(self.current_record, legal_actions)

                                if act["pai"][-1] == "r" and act["pai"][:2] in self.tehais:
                                        act["pai"] = act["pai"][:2]
                                        act["tsumogiri"] = True if self.prev_tsumo == act["pai"] else False
                
                                response = act

                        elif (action["type"] == "dahai" or action["type"] == "kakan") and action["actor"] == self.id: #---tehai change---
                                self.current_record.append(action)
                                self.tehais.remove(action["pai"])
                                response = {"type":"none"}
                        elif (action["type"] == "daiminkan" or action["type"] == "ankan") and action["actor"] == self.id:
                                self.current_record.append(action)
                                for hai in action["consumed"]:
                                        self.tehais.remove(hai)
                                response = {"type":"none"}

                        elif action["type"] == "dahai" and action["actor"] != self.id:#fuuro
                                self.current_record.append(action)
                                if len(action["possible_actions"]) > 0:
                                        legal_actions = action["possible_actions"]
                                        response = self.ai.choose_action(self.current_record, legal_actions)
                                        legal_actions = []
                                else:
                                        response = {"type":"none"}
                        else:
                                self.current_record.append(action)
                                response = {"type":"none"}
                        #print("rec: ", current_record)
                        print("->\t" + json.dumps(response))
                        self.socket.sendall(bytes(json.dumps(response)+"\n", "utf-8"))

if __name__ == "__main__":
        client = MJAI_Client()
        client.run()
