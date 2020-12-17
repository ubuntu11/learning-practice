import random as r

def generate_list(lower:int, upper: int)->list:
    return [i for i in range(lower, upper)]

def shuffle_list(o_list: list)->list:
    n_list = [i for i in o_list]
    for k,v in enumerate(n_list):
      idx = r.randint(0, len(n_list)-1)
      n_list[k], n_list[idx] = n_list[idx], v
    print(n_list)  
    return n_list 
