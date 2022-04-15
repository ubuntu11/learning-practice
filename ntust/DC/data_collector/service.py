from datetime import date, datetime
import psutil
from data_access import insert_doc, connect_to_db
from model import equipment
from getpass import getuser


def update_status(db_client):
    computer_state = {'ID':'dc',
                'time':0,
                'boot_time':0,
                'cpu':0,
                'memory':{'total':'0' ,'mem_per':0},
                'IP':1,
                'mongoState':-1
                }
    try:
        # region computer state
        now = datetime.now()
        user = getuser()
        cpu = psutil.cpu_percent(interval = 0.3)
        memory = psutil.virtual_memory()
        mem = str(round(memory[0] / 1073741824 ,2))
        mem_per = round(memory[2] / 100, 3)
        boot_time = datetime.fromtimestamp(psutil.boot_time())

        computer_state['ID'] = user
        computer_state['time'] = now
        computer_state['boot_time'] = boot_time
        computer_state['cpu'] = cpu
        computer_state['memory']['total'] = mem
        computer_state['memory']['mem_per'] = mem_per
        # print(computer_state)

        if not insert_doc(db_client, 'pc_info' , computer_state , 'Equipment_Status'):
            print('Computer State upload fail')

        for key in equipment.keys():
            if equipment[key]:
                if not db_client['AFC']['equipment_status'].update_one({'name':key},{'$set':{'count':0}}):
                    print('Equipment status update fail : {}'.format(key))
    except Exception as meg:
        print('Equipment_Status error')
        print(meg)