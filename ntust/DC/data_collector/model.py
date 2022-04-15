TMM70={ 'read_data':{'function_code':0x03,'start':[304,385],'end':[358,402]},
        'v_a':  {'order':0,'address':305,'length':2,'type':'uint','scale':10},
        'v_b':  {'order':0,'address':307,'length':2,'type':'uint','scale':10},
        'v_c':  {'order':0,'address':309,'length':2,'type':'uint','scale':10},
        'v':    {'order':0,'address':311,'length':2,'type':'uint','scale':10},
        'vl_ab':{'order':0,'address':313,'length':2,'type':'uint','scale':10},
        'vl_bc':{'order':0,'address':315,'length':2,'type':'uint','scale':10},
        'vl_ca':{'order':0,'address':317,'length':2,'type':'uint','scale':10},
        'vl':   {'order':0,'address':319,'length':2,'type':'uint','scale':10},
        'i_a':  {'order':0,'address':321,'length':2,'type':'uint','scale':1000},
        'i_b':  {'order':0,'address':323,'length':2,'type':'uint','scale':1000},
        'i_c':  {'order':0,'address':325,'length':2,'type':'uint','scale':1000},
        'i':    {'order':0,'address':327,'length':2,'type':'uint','scale':1000},
        'p_a':  {'order':0,'address':331,'length':2,'type':'int' ,'scale':1000},
        'p_b':  {'order':0,'address':333,'length':2,'type':'int' ,'scale':1000},
        'p_c':  {'order':0,'address':335,'length':2,'type':'int' ,'scale':1000},
        'p':    {'order':0,'address':337,'length':2,'type':'int' ,'scale':1000},
        'q_a':  {'order':0,'address':339,'length':2,'type':'int' ,'scale':1000},
        'q_b':  {'order':0,'address':341,'length':2,'type':'int' ,'scale':1000},
        'q_c':  {'order':0,'address':343,'length':2,'type':'int' ,'scale':1000},
        'q':    {'order':0,'address':345,'length':2,'type':'int' ,'scale':1000},
        's_a':  {'order':0,'address':347,'length':2,'type':'uint','scale':1000},
        's_b':  {'order':0,'address':349,'length':2,'type':'uint','scale':1000},
        's_c':  {'order':0,'address':351,'length':2,'type':'uint','scale':1000},
        's':    {'order':0,'address':353,'length':2,'type':'uint','scale':1000},
        'pf_a': {'order':0,'address':355,'length':1,'type':'int' ,'scale':1000},
        'pf_b': {'order':0,'address':356,'length':1,'type':'int' ,'scale':1000},
        'pf_c': {'order':0,'address':357,'length':1,'type':'int' ,'scale':1000},
        'pf':   {'order':0,'address':358,'length':1,'type':'int' ,'scale':1000},
        'f':    {'order':0,'address':304,'length':1,'type':'uint','scale':100 },
        'imp_kwh':  {'order':1,'address':385,'length':2,'type':'uint','scale':10},
        'exp_kwh':  {'order':1,'address':387,'length':2,'type':'uint','scale':10},
        'tot_kwh':  {'order':1,'address':389,'length':2,'type':'uint','scale':10},
        'net_kwh':  {'order':1,'address':391,'length':2,'type':'int' ,'scale':10},
        'imp_kvarh':{'order':1,'address':393,'length':2,'type':'uint','scale':10},
        'exp_kvarh':{'order':1,'address':395,'length':2,'type':'uint','scale':10},
        'tot_kvarh':{'order':1,'address':397,'length':2,'type':'uint','scale':10},
        'net_kvarh':{'order':1,'address':399,'length':2,'type':'int' ,'scale':10},
        'imp_kvah': {'order':None,'address':305,'length':2,'type':'uint','scale':10},
        'exp_kvah': {'order':None,'address':305,'length':2,'type':'uint','scale':10},
        'tot_kvah': {'order':1,'address':401,'length':2,'type':'uint','scale':10},
        'net_kvah': {'order':None,'address':305,'length':2,'type':'uint','scale':10},
}

S42={ 'read_data':{'function_code':0x03,'start':[0x1010,0x1020,0x1030],'end':[0x101F,0x102F,0x103F]},
        'protection_50_51_1':   {'order':0,'address':0x1010,'length':1,'type':'uint','scale':1},
        'protection_50_51_2':   {'order':0,'address':0x1011,'length':1,'type':'uint','scale':1},
        'protection_50_51_3':   {'order':0,'address':0x1012,'length':1,'type':'uint','scale':1},
        'protection_50_51_4':   {'order':0,'address':0x1013,'length':1,'type':'uint','scale':1},
        'protection_50N_51N_1': {'order':0,'address':0x1014,'length':1,'type':'uint','scale':1},
        'protection_50N_51N_2': {'order':0,'address':0x1015,'length':1,'type':'uint','scale':1},
        'protection_50N_51N_3': {'order':0,'address':0x1016,'length':1,'type':'uint','scale':1},
        'protection_50N_51N_4': {'order':0,'address':0x1017,'length':1,'type':'uint','scale':1},
        'protection_81H_1':     {'order':1,'address':0x1029,'length':1,'type':'uint','scale':1},
        'protection_81H_2':     {'order':1,'address':0x102A,'length':1,'type':'uint','scale':1},
        'protection_81L_1':     {'order':1,'address':0x102B,'length':1,'type':'uint','scale':1},
        'protection_81L_2':     {'order':1,'address':0x102C,'length':1,'type':'uint','scale':1},
        'protection_81L_3':     {'order':1,'address':0x102D,'length':1,'type':'uint','scale':1},
        'protection_81L_4':     {'order':1,'address':0x102E,'length':1,'type':'uint','scale':1},
        'protection_27_27S_1':  {'order':1,'address':0x1022,'length':1,'type':'uint','scale':1},
        'protection_27_27S_2':  {'order':1,'address':0x1023,'length':1,'type':'uint','scale':1},
        'protection_67_1':      {'order':2,'address':0x1030,'length':1,'type':'uint','scale':1},
        'protection_67_2':      {'order':2,'address':0x1031,'length':1,'type':'uint','scale':1},
        'protection_59_1':      {'order':1,'address':0x1025,'length':1,'type':'uint','scale':1},
        'protection_59_2':      {'order':1,'address':0x1026,'length':1,'type':'uint','scale':1},
        'protection_59N_1':     {'order':1,'address':0x1027,'length':1,'type':'uint','scale':1},
        'protection_59N_2':     {'order':1,'address':0x1028,'length':1,'type':'uint','scale':1},
}

PM_Data = { 'ID':'60f8e3e654841d42146b27b5',
            'v_a':0,
            'v_b':0,
            'v_c':0,
            'v':0,
            'vl_ab':0,
            'vl_bc':0,
            'vl_ca':0,
            'vl':0,
            'i_a':0,
            'i_b':0,
            'i_c':0,
            'i':0,
            'p_a':0,
            'p_b':0,
            'p_c':0,
            'p':0,
            'q_a':0,
            'q_b':0,
            'q_c':0,
            'q':0,
            's_a':0,
            's_b':0,
            's_c':0,
            's':0,
            'pf_a':0,
            'pf_b':0,
            'pf_c':0,
            'pf':0,
            'f':0,
            'imp_kwh':0,
            'exp_kwh':0,
            'tot_kwh':0,
            'net_kwh':0,
            'imp_kvarh':0,
            'exp_kvarh':0,
            'tot_kvarh':0,
            'net_kvarh':0,
            'imp_kvah':None,
            'exp_kvah':None,
            'tot_kvah':0,
            'net_kvah':None,
            }

LM_Data = { 'ID':'60f8e3c854841d42146b2761',
            'v_a':0,
            'v_b':0,
            'v_c':0,
            'v':0,
            'vl_ab':0,
            'vl_bc':0,
            'vl_ca':0,
            'vl':0,
            'i_a':0,
            'i_b':0,
            'i_c':0,
            'i':0,
            'p_a':0,
            'p_b':0,
            'p_c':0,
            'p':0,
            'q_a':0,
            'q_b':0,
            'q_c':0,
            'q':0,
            's_a':0,
            's_b':0,
            's_c':0,
            's':0,
            'pf_a':0,
            'pf_b':0,
            'pf_c':0,
            'pf':0,
            'f':0,
            'imp_kwh':0,
            'exp_kwh':0,
            'tot_kwh':0,
            'net_kwh':0,
            'imp_kvarh':0,
            'exp_kvarh':0,
            'tot_kvarh':0,
            'net_kvarh':0,
            'imp_kvah':None,
            'exp_kvah':None,
            'tot_kvah':0,
            'net_kvah':None,
            }

equipment = {'門禁':1,
            'DI1':1,
            'DI2':1,
            '低壓側電表':1,
            '輔助電源電表':1,
            '環境數據':1,
            '保護電驛':1,
            'EM_STOP':1,
            }
