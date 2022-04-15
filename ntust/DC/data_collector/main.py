import sys


from time import sleep
from datetime import date, datetime

from pyModbusTCP.client import ModbusClient
from apscheduler.schedulers.blocking import BlockingScheduler
from apscheduler.triggers.combining import OrTrigger
from apscheduler.triggers.cron import CronTrigger
from threading import Thread

# import models
from model import TMM70, S42, PM_Data, LM_Data, equipment
# import mongodb related functions
from data_access import insert_doc, connect_to_db
from service import update_status
from access_control_server import start_server as start_access_tcp_server


def Modbus_Device_Reading_1_1():
    return True


def Modbus_Device_Reading_1_2():
    return True


def Modbus_Device_Reading_1_3():
    return True


def Modbus_Device_Reading_2():
    return True


def Modbus_Device_Reading_3():
    return True


def EM_control():
    return True


def main():
    print ('start main thread')
    one_second_trigger = OrTrigger([CronTrigger(second='*/1')])
    three_second_trigger = OrTrigger([CronTrigger(second='*/3')])
    five_second_trigger = OrTrigger([CronTrigger(second='*/5')])
    ten_second_trigger = OrTrigger([CronTrigger(second='*/10')])
    twenty_second_trigger = OrTrigger([CronTrigger(second='*/20')])
    one_minute_trigger = OrTrigger([CronTrigger(minute='*/1')])

    try:
        for key in TMM70.keys():
            if (key!='read_data') and ( TMM70[key]['order']!=None ):
                TMM70[key]['return_address']=TMM70[key]['address']-TMM70['read_data']['start'][ TMM70[key]['order'] ]
            elif (key=='read_data'):
                TMM70['read_data']['length']=[]
                for x in range(len(TMM70['read_data']['start'])):
                    TMM70['read_data']['length'].append( TMM70['read_data']['end'][x]-TMM70['read_data']['start'][x]+1 )
    except Exception as meg:
        print('TMM70 read_data error')
        print(meg)
    # print (TMM70['read_data'])
    try:
        for key in S42.keys():
            if (key!='read_data') and ( S42[key]['order']!=None ):
                S42[key]['return_address']=S42[key]['address']-S42['read_data']['start'][ S42[key]['order'] ]
            elif (key=='read_data'):
                S42['read_data']['length']=[]
                for x in range(len(S42['read_data']['start'])):
                    S42['read_data']['length'].append( S42['read_data']['end'][x]-S42['read_data']['start'][x]+1 )
    except Exception as meg:
        print('S42 read_data error')
        print(meg)
    # print (S42['read_data'])
    # print (S42)

    try:
        db_client = connect_to_db('127.0.0.1:27017', redundant = False)
        if not db_client:
            sys.exit('fail to connect to mongodb')

        # 控制線程
        # DO_control = Thread(target = EM_control)
        # DO_control.start()
        # 門禁系統讀取線程
        AR = Thread(target=start_access_tcp_server, args=(db_client,))
        AR.start()
        # 各副程式排程執行 |
        scheduler = BlockingScheduler({'apscheduler.job_defaults.max_instances':'50'})
        scheduler.add_job(update_status, five_second_trigger, args=(db_client,))
        # sched.add_job(Modbus_Device_Reading_1_1, FiveSecond)
        # sched.add_job(Modbus_Device_Reading_1_2, FiveSecond)
        # sched.add_job(Modbus_Device_Reading_1_3, OneSecond)3

        # sched.add_job(Modbus_Device_Reading_2, OneSecond)
        # sched.add_job(Modbus_Device_Reading_3, OneSecond)
        scheduler.start()

    except Exception as meg:
        print('main error')
        print(meg)


if __name__=='__main__':
    main()
