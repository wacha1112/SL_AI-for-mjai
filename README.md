# Usage
You have to install mjai(https://github.com/gimite/mjai).
Put SL_AI.py, lib(<https://github.com/critter-mj/akochan_ui/tree/master/lib>), supervised_model(https://github.com/critter-mj/akochan_ui/tree/master/supervised_model), in same directory. And make log directory in the directory.
Then, in lib/ml_module.py, you have to change line 103
``` self.model_paths['kan'] = 'supervised_model/kan_model_cpu_state_dict.pth' ```
to
```
self.model_paths['ankan'] = 'supervised_model/kan_model_cpu_state_dict.pth'
self.model_paths['kakan'] = 'supervised_model/kan_model_cpu_state_dict.pth'
self.model_paths['daiminkan'] = 'supervised_model/kan_model_cpu_state_dict.pth'
```
You can start game by executing following in the directory.
```
mjai server --port=11600 --game_type=tonpu --log_dir=log --games=5 "python3 SL_AI.py" "python3 SL_AI.py" "python3 SL_AI.py" "python3 SL_AI.py"
```
