# Ver 0.0.1 20210315-20210408
# 需加入資料補遺之功能

# region defind module
import socket
import os
import psutil
# import clr
# clr.AddReference("RSNetDevice")
from time import sleep
from struct import unpack
from math import isnan
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
# from RSNetDevice import *
from datetime import date, datetime
from getpass import getuser
from pyModbusTCP.client import ModbusClient
from apscheduler.schedulers.blocking import BlockingScheduler
from apscheduler.triggers.combining import OrTrigger
from apscheduler.triggers.cron import CronTrigger
from threading import Thread
# endregion
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
# endregion

# def print(text):
#     try:
#         now = datetime.now()
#         with open('System_log.txt' ,'a+') as file:
#             file.write(text + '  {}\n'.format(str(now)))
#         print(text)
#     except Exception as meg:
#         print('print')
#         print(meg)

def Equipment_Status():
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
        if not MongoDataBase_Uploading('pc_info' ,computer_state ,'Equipment_Status'):
            print('Computer State upload fail') 
        # endregion
        # print (equipment)
        for key in equipment.keys():
            if equipment[key]:
                # print(key)
                if not mongo_client_connect['AFC']['equipment_status'].update({'name':key},{'$set':{'count':0}}):
                    print('Equipment status update fail : {}'.format(key))
    except Exception as meg:
        print('Equipment_Status error')
        print(meg)

def Bytes_Array_To_Float32(value_high ,value_low):
    try:
        total = (value_high << 16) + value_low
        total = format(total, 'x')
        total = str(total)
        if len(total) < 8:#須做補0動作 ex:100=>0100
            sub = 8 - len(total)
            for x in range(0 ,sub):
                total = '0' + total
        else:
            pass#無須做補0動作 ex:1000
        total = bytes.fromhex(total)
        total = unpack('>f', total)[0]
        if isnan(total):
            total = False
        else:
            total = float(total)
        return total
    except Exception as meg:
        print('BytearrayToFloat32 error')
        print(meg)
        return False
def Bytes_Array_To_Uint(value_array ,point_data):
    try:
        total=value_array[point_data['return_address']]
        for x in range(1,point_data['length']):
            total=(total<<16)+value_array[point_data['return_address']+x]
        if isnan(total):
            total = False
        else:
            total = float(total)/point_data['scale']
        return total
    except Exception as meg:
        print('BytearrayToInt error')
        print(meg)
        return False
def Bytes_Array_To_Int(value_array,point_data):
    try:
        total=value_array[point_data['return_address']]
        for x in range(1,point_data['length']):
            total=(total<<16)+value_array[point_data['return_address']+x]
        xorbit=256**(point_data['length']*2)-1 #TCP length*2
        xorbit_test=(256**(point_data['length']*2))>>1 #TCP length*2
        total_test=int(total)^xorbit_test
        if int(total)>int(total_test):
            total=-(( (int(total)-1) ^xorbit))
        else :
            pass
        if isnan(total):
            total = False
        else:
            total = float(total)/point_data['scale']
        return total
    except Exception as meg:
        print('BytearrayToUint error')
        print(meg)
        return False
def DataBase_Connect(host ,user = 'root' ,password = 'pc152' ,redundant = True):
    try:
        count=0 
        while True:
            global mongo_client_connect
            if redundant:
                mongo_client_connect = MongoClient(host ,replicaset = 'rs0' ,serverSelectionTimeoutMS = 10000)
            else:
                mongo_client_connect = MongoClient(host ,serverSelectionTimeoutMS = 10000)
            try:
                mongo_client_connect.admin.authenticate(user,password)
            except ConnectionFailure as err_msg:
                count = count + 1
                print(str(err_msg) + '. {} times'.format(count))
            else:
                break
            if count == 3:
                print('MongoDB Server not found yet!')
                return False
        return True
    except Exception as meg:
        print('DataBase_Connect error')
        print(meg)
        return False

def MongoDataBase_Uploading(collection ,data ,program_name ,db = 'AFC'  ,num = 'one'):
    try:
        if num == 'one':
            upload_status = mongo_client_connect[db][collection].insert_one(data)
        elif num == 'many':
            upload_status = mongo_client_connect[db][collection].insert_many(data)
        else:
            print('Enter wrong num!')
        if not upload_status:
            print('Mongo DataBase Upload Fail :' + program_name)
            return False
        return True
    except Exception as meg:
        print('MongoDataBase_Uploading error')
        print(meg)
        return False

# It's need to upload to database, so I need to programing about upload past.
def Access_Reading():
    try:
        # HOST = '192.168.2.1'
        HOST = '192.168.56.2'
        # PORT = 8031
        PORT = 502
        count = 0
        while True:
            try:
                server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server.bind((HOST, PORT))
                server.listen(10)
                break
            except Exception as meg:
                count += 1
                print(str(meg) + '{} times'.format(count))
                sleep(3)

        while True:
            conn, addr = server.accept()
            clientMessage = str(conn.recv(1024), encoding='utf-8')
            access = {'ID':'60f8eaf254841d42146b3af2', 'cardNumber':'0', 'cardName':'0', 'event':'0', 'time':datetime.now()}
            buffer = []
            for i in range(0, len(clientMessage)):
                if clientMessage[len(clientMessage) - i - 1] == ')':
                    buffer.append(i)
            event_count = (len(clientMessage) - buffer[0])
            number_count = (len(clientMessage) - buffer[1])
            Mess_len = len(clientMessage)
            access['event'] = clientMessage[event_count : Mess_len - 1]
            if access['event'] == 'Normal Access' or access['event'] == 'Invalid card':
                access['cardNumber'] = clientMessage[number_count : number_count + 11]
                if access['cardNumber'] == '02900:00000':
                    access['cardName'] = 'Mnamel Xxx'
                elif access['cardNumber'] == '18000:00000':
                    access['cardName'] = 'XX Admin_1'
                elif access['cardNumber'] == '17000:00000':
                    access['cardName'] = 'XX Admin_2'
                elif access['cardNumber'] == '15000:00000':
                    access['cardName'] = 'XXXX Admin'
                else:
                    access['cardName'] = 'No Supported'
            else:
                access['cardNumber'] = 'None'
                access['cardName'] = 'None'
            print(access)
            if MongoDataBase_Uploading(collection = 'access',data = access ,program_name = 'Access_Reading'):
                print('Access_Uploading Seccessful')
            else:
                print('Access_Uploading fail')
        
            conn.close()
        return True
    except Exception as meg:
        print('Access_Reading error')
        print(meg)
        conn.close()
        return False 

def Modbus_Device_Reading_1_1():
    try:
        # print ('Modbus_Device_Reading_1_1 start')
        now_time = datetime.now()
        count = 0
        # PM = ModbusClient(host = '192.168.56.2' ,port = 502 ,unit_id = 1 ,timeout = 1 ,auto_open = True ,auto_close = True)
        # LM = ModbusClient(host = '192.168.56.2' ,port = 502 ,unit_id = 2 ,timeout = 1 ,auto_open = True ,auto_close = True)
        #---------------------------test_start------------------------------
        PM = ModbusClient(host = '127.0.0.1' ,port = 502 ,unit_id = 1 ,timeout = 1 ,auto_open = True ,auto_close = True)
        LM = ModbusClient(host = '127.0.0.1' ,port = 502 ,unit_id = 2 ,timeout = 1 ,auto_open = True ,auto_close = True)
        #---------------------------test_end--------------------------------
       
        #------------------------------------------------------------------------------------------------
        # region Power Meter Reading
        result=[[],[]]
        while True:
            result[0] = PM.read_holding_registers(TMM70['read_data']['start'][0] ,TMM70['read_data']['length'][0])
            result[1] = PM.read_holding_registers(TMM70['read_data']['start'][1] ,TMM70['read_data']['length'][1])
            # print (result[0])
            # print (result[1])
            if (result[0]!= None) & (result[1] != None):
                equipment['輔助電源電表'] = 1
                if '_id' in PM_Data:
                    del PM_Data['_id']
                    del PM_Data['time']
                for key in PM_Data.keys():
                    # print (key)
                    if (key != 'ID') & (key != 'time'):
                        if TMM70[key]['order']!=None:
                            if TMM70[key]['type']=='uint':
                                PM_Data[key] = Bytes_Array_To_Uint(result[ TMM70[key]['order'] ] ,TMM70[key])
                            elif TMM70[key]['type']=='int':
                                PM_Data[key] = Bytes_Array_To_Int(result[ TMM70[key]['order'] ] ,TMM70[key])
                PM_Data['time'] = now_time
                #---------------------------test_start------------------------------
                # print(PM_Data)
                # for key in PM_Data.keys():
                #     print (key+' :'+str(PM_Data[key]))
                #---------------------------test_end--------------------------------
                MongoDataBase_Uploading(collection = 'acm',data = PM_Data ,program_name = 'Modbus_Device_Reading_1_1')
                break
            else:
                count += 1
                print("Power Meter reading fail {} times".format(count))
                if count >= 1:
                    equipment['輔助電源電表'] = 0
                    count = 0
                    break
        # endregion
        #------------------------------------------------------------------------------------------------
        # region Power Meter Reading
        result=[[],[]]
        while True:
            result[0] = LM.read_holding_registers(TMM70['read_data']['start'][0] ,TMM70['read_data']['length'][0])
            result[1] = LM.read_holding_registers(TMM70['read_data']['start'][1] ,TMM70['read_data']['length'][1])
            # print (result[0])
            # print (result[1])
            if (result[0]!= None) & (result[1] != None):
                equipment['低壓側電表'] = 1
                if '_id' in LM_Data:
                    del LM_Data['_id']
                    del LM_Data['time']
                for key in LM_Data.keys():
                    if (key != 'ID') & (key != 'time'):
                        if TMM70[key]['order']!=None:
                            if TMM70[key]['type']=='uint':
                                LM_Data[key] = Bytes_Array_To_Uint(result[ TMM70[key]['order'] ] ,TMM70[key])
                            elif TMM70[key]['type']=='int':
                                LM_Data[key] = Bytes_Array_To_Int(result[ TMM70[key]['order'] ] ,TMM70[key])
                LM_Data['time'] = now_time
                #---------------------------test_start------------------------------
                # print(LM_Data)
                # for key in LM_Data.keys():
                #     print (key+' :'+str(LM_Data[key]))
                #---------------------------test_end--------------------------------
                MongoDataBase_Uploading(collection = 'acm',data = LM_Data ,program_name = 'Modbus_Device_Reading_1_1')
                break
            else:
                count += 1
                print("Low Voltage Meter reading fail {} times".format(count))
                if count >= 1:
                    equipment['低壓側電表'] = 0
                    count = 0
                    break
        # endregion
    except Exception as meg:
        print('Modbus_Device_Reading_1_1 error')
        print(meg)
        return False
def Modbus_Device_Reading_1_2():
    try:
        # print ('Modbus_Device_Reading_1_2 start')
        now_time = datetime.now()
        count = 0
        data = [{'ID':'60f8eaf254841d42146b3af5' ,'temp':0},
        ]
        # MT6200 = ModbusClient(host = '192.168.56.2' ,port = 502 ,unit_id = 1 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
        #---------------------------test_start------------------------------
        MT6200 = ModbusClient(host = '127.0.0.1' ,port = 503 ,unit_id = 1 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
        #---------------------------test_end--------------------------------
        #------------------------------------------------------------------------------------------------
        # region MT6200 Reading
        while True:
            result = MT6200.read_input_registers(0x1000, 1)
            # print (result)
            if result != None:
                
                equipment['環境數據'] = 1
                data[0]['temp']=result[0]
                data[0]['time'] = now_time
                #---------------------------test_start------------------------------
                # print(data)
                #---------------------------test_end--------------------------------
                #------------------------------------------------------------------------------------------------
                MongoDataBase_Uploading(collection = 'env',data = data ,num = 'many',program_name = 'Modbus_Device_Reading_1_2')
                #------------------------------------------------------------------------------------------------
                break
            else:
                count += 1
                print('MT6200 reading fail: {} times'.format(count))
                if count >= 4:
                    equipment['環境數據'] = 0
                    count = 0
                    break
        # endregion
        #------------------------------------------------------------------------------------------------
        return True
    except Exception as meg:
        print('Modbus_Device_Reading_1_2 error')
        print(meg)
        return False
def Modbus_Device_Reading_1_3():
    try:
        # print ('Modbus_Device_Reading_1_3 start')
        now_time = datetime.now()
        count = 0
        data = [{'ID':'60f8eaf254841d42146b3aff' ,'acb':0,'vcb':0},
        ]
        # ADAM4055 = ModbusClient(host = '192.168.56.2' ,port = 502 ,unit_id = 3 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
        #---------------------------test_start------------------------------
        ADAM4055 = ModbusClient(host = '127.0.0.1' ,port = 504 ,unit_id = 3 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
        #---------------------------test_end--------------------------------
        #------------------------------------------------------------------------------------------------
        # region ADAM4055 Reading
        while True:
            result = ADAM4055.read_holding_registers(0x0000, 1)
            # print (result)
            if result != None:
                byte_data=result[0]
                try:
                    binlist=list(bin(byte_data))
                    del binlist[0:2]
                    binlistint=list(map(lambda x:int(x),binlist))
                    while True:
                        if len(binlistint)<16:
                            binlistint.insert(0,0)
                        else:
                            break
                    binlistint.reverse()
                except Exception as meg:
                    print('ADAM4055 error')
                    print(meg)
                    break
                # print(binlistint)
                equipment['DI2'] = 1
                if(binlistint[0]==1) & (binlistint[1]==0) & (binlistint[2]==0):
                    data[0]['acb'] = 0
                elif (binlistint[0]==0) & (binlistint[1]==1) & (binlistint[2]==0):
                    data[0]['acb'] = 1
                elif (binlistint[2]==1):
                    data[0]['acb'] = 2
                elif(binlistint[0]==0) & (binlistint[1]==0) & (binlistint[2]==0):
                    data[0]['acb'] = 0
                elif (binlistint[0]==1) & (binlistint[1]==1) & (binlistint[2]==0):
                    data[0]['acb'] = 1
                else:
                    data[0]['acb'] = 2

                if(binlistint[3]==1) & (binlistint[4]==0) & (binlistint[5]==0):
                    data[0]['vcb'] = 0
                elif (binlistint[3]==0) & (binlistint[4]==1) & (binlistint[5]==0):
                    data[0]['vcb'] = 1
                elif (binlistint[5]==1):
                    data[0]['vcb'] = 2
                elif(binlistint[3]==0) & (binlistint[4]==0) & (binlistint[5]==0):
                    data[0]['vcb'] = 0
                elif (binlistint[3]==1) & (binlistint[4]==1) & (binlistint[5]==0):
                    data[0]['vcb'] = 1
                else:
                    data[0]['vcb'] = 2
                data[0]['time'] = now_time
                #---------------------------test_start------------------------------
                # print(data)
                # for key in data[0].keys():
                #     print (key+' :'+str(data[0][key]))
                #---------------------------test_end--------------------------------
                #------------------------------------------------------------------------------------------------
                MongoDataBase_Uploading(collection = 'rio',data = data ,num = 'many',program_name = 'Modbus_Device_Reading_1_3')
                #------------------------------------------------------------------------------------------------
                break
            else:
                count += 1
                print('ADAM4055 reading fail: {} times'.format(count))
                if count >= 4:
                    equipment['DI2'] = 0
                    count = 0
                    break
        # endregion
        #------------------------------------------------------------------------------------------------
        return True
    except Exception as meg:
        print('Modbus_Device_Reading_1_3 error')
        print(meg)
        return False

def Modbus_Device_Reading_2():
    try:
        # print ('Modbus_Device_Reading_2 start')
        now_time = datetime.now()
        count = 0
        data = [{'ID':'60f8eaf254841d42146b3afc' ,
        'ready':0 ,
        'comm_alarm':0,
        'warning':0,
        'fault':0,
        'sp_ac_input_mccb_trip':0,
        'em_stop_out':0,},
        ]
        # ADAM6251 = ModbusClient(host = '192.168.2.7' ,port = 502 ,unit_id = 1 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
        #---------------------------test_start------------------------------
        ADAM6251 = ModbusClient(host = '127.0.0.1' ,port = 505 ,unit_id = 1 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
        #---------------------------test_end--------------------------------
        #------------------------------------------------------------------------------------------------
        # region ADAM6251 Reading
        while True:
            result = ADAM6251.read_holding_registers(0x0000, 1)
            if result != None:
                byte_data=result[0]
                try:
                    binlist=list(bin(byte_data))
                    del binlist[0:2]
                    binlistint=list(map(lambda x:int(x),binlist))
                    while True:
                        if len(binlistint)<16:
                            binlistint.insert(0,0)
                        else:
                            break
                    binlistint.reverse()
                except Exception as meg:
                    print('ADAM6251 error')
                    print(meg)
                    break
                # print (binlistint)
                equipment['DI1'] = 1
                if (binlistint[0]==1):
                    data[0]['ready'] = 1
                else:
                    data[0]['ready'] = 0
                if (binlistint[1]==1):
                    data[0]['comm_alarm'] = 1
                else:
                    data[0]['comm_alarm'] = 0
                if (binlistint[2]==1):
                    data[0]['warning'] = 1
                else:
                    data[0]['warning'] = 0
                if (binlistint[3]==1):
                    data[0]['fault'] = 1
                else:
                    data[0]['fault'] = 0
                if (binlistint[4]==1):
                    data[0]['sp_ac_input_mccb_trip'] = 1
                else:
                    data[0]['sp_ac_input_mccb_trip'] = 0
                if (binlistint[5]==1):
                    data[0]['em_stop_out'] = 1
                else:
                    data[0]['em_stop_out'] = 0
                data[0]['time'] = now_time
                #---------------------------test_start------------------------------
                # print(data)
                # for key in data[0].keys():
                #     print (key+' :'+str(data[0][key]))
                #---------------------------test_end--------------------------------
                #------------------------------------------------------------------------------------------------
                MongoDataBase_Uploading(collection = 'rio',data = data ,num = 'many',program_name = 'Modbus_Device_Reading_2')
                #------------------------------------------------------------------------------------------------
                break
            else:
                count += 1
                print('ADAM6251 reading fail: {} times'.format(count))
                if count >= 4:
                    equipment['DI1'] = 0
                    count = 0
                    break
        # endregion
        #------------------------------------------------------------------------------------------------
        return True
    except Exception as meg:
        print('Modbus_Device_Reading_2 error')
        print(meg)
        return False
def Modbus_Device_Reading_3():
    count = 0
    now_time = datetime.now()
    data = [{'ID':'60f8eba954841d42146b3cf8',
    'protection_50_51_1':0,
    'protection_50_51_2':0,
    'protection_50_51_3':0,
    'protection_50_51_4':0,
    'protection_50N_51N_1':0,
    'protection_50N_51N_2':0,
    'protection_50N_51N_3':0,
    'protection_50N_51N_4':0,
    'protection_81H_1':0,
    'protection_81H_2':0,
    'protection_81L_1':0,
    'protection_81L_2':0,
    'protection_81L_3':0,
    'protection_81L_4':0,
    'protection_27_27S_1':0,
    'protection_27_27S_2':0,
    'protection_67_1':0,
    'protection_67_2':0,
    'protection_59_1':0,
    'protection_59_2':0,
    'protection_59N_1':0,
    'protection_59N_2':0,
    }
    ]
    #------------------------------------------------------------------------------------------------
    # region S42 Reading
    # 保護電驛
    try:
        # S42_TCP = ModbusClient(host = '192.168.2.7' ,port = 502 ,unit_id = 1 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
        #---------------------------test_start------------------------------
        S42_TCP = ModbusClient(host = '127.0.0.1' ,port = 506 ,unit_id = 1 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
        #---------------------------test_end--------------------------------
        result=[[],[],[]]
        while True:
            # print (S42['read_data'])
            result[0] = S42_TCP.read_holding_registers(S42['read_data']['start'][0] ,S42['read_data']['length'][0])
            result[1] = S42_TCP.read_holding_registers(S42['read_data']['start'][1] ,S42['read_data']['length'][1])
            result[2] = S42_TCP.read_holding_registers(S42['read_data']['start'][2] ,S42['read_data']['length'][2])
            # print (result)
            if (result[0] != None) and (result[1] != None) and (result[2] != None):
                equipment['保護電驛'] = 1
                for key in data[0].keys():
                    if (key != 'ID') & (key != 'time') :
                        if S42[key]['order']!=None:
                            if result[ S42[key]['order'] ][ S42[key]['return_address'] ]==1:
                                data[0][key] = 1
                            else:
                                data[0][key] = 0
                data[0]['time'] = now_time
                #---------------------------test_start------------------------------
                # for key in data[0].keys():
                #     print (key+' :'+str(data[0][key]))
                #---------------------------test_end--------------------------------

                #------------------------------------------------------------------------------------------------
                MongoDataBase_Uploading(collection = 'protective_relay',data = data ,num = 'many',program_name = 'Modbus_Device_Reading_3')
                #------------------------------------------------------------------------------------------------
                break
            else:
                count += 1
                print('SEPAMS42 reading fail: {} times'.format(count))
                if count >= 4:
                    equipment['保護電驛'] = 0
                    count = 0
                    break
        # endregion
    except Exception as meg:
        print('Modbus_Device_Reading_3')
        print(meg)
        return False
def EM_control():
    EM_Status=10
    count=0
    # AM6256 = ModbusClient(host = '192.168.2.7' ,port = 502 ,unit_id = 4 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
    #---------------------------test_start------------------------------
    AM6256 = ModbusClient(host = '127.0.0.1' ,port = 510 ,unit_id = 1 ,timeout = 0.1 ,auto_open = True ,auto_close = True)
    #---------------------------test_end--------------------------------
    while True:
        try:
            #------------------------------------------------------------------------------------------------
            for DB_Result in mongo_client_connect['AFC']['em_control'].find({'ID':'6055e92d7445a0a1cb1bb42c'}).sort('time' ,-1).limit(1):
                # print(DB_Result)
                # print(EM_Status)
                # print (int(DB_Result['control']))
                if int(DB_Result['control']) != EM_Status:
                    while True:
                        if DB_Result['control'] == 1:
                            write_data = 0x0000
                            write_check = True
                        else:
                            write_data = 0x0001
                            write_check = False
                        if AM6256.write_single_register(0x0000 ,write_data):
                            result = AM6256.read_holding_registers(0x0000 ,1)
                            print(result)
                            if (not result[0]) == write_check:
                                equipment['EM_STOP'] = 1
                                EM_Status = DB_Result['control']
                                print('-------------------------')
                                print('-------------------------')
                                print('-------------------------')
                                print('EM_STOP control successfully')
                                print('-------------------------')
                                print('-------------------------')
                                print('-------------------------')
                                break
                        else:
                            count += 1
                            print('EM_STOP control fail: {} times'.format(count))
                            if count >= 4:
                                equipment['EM_STOP'] = 0
                                count = 0
                                break
                else:
                    while True:
                        result = AM6256.read_holding_registers(0x0000 ,1)
                        if result != None:
                            equipment['EM_STOP'] = 1
                            break
                        else:
                            count += 1
                            if count >= 4:
                                equipment['EM_STOP'] = 0
                                count = 0
                                break
        except Exception as meg:
            print('EM_control error')
            print(meg)
        sleep(1)
    #------------------------------------------------------------------------------------------------
def main():
    print ('main start')
    # region
    OneSecond = OrTrigger([
        CronTrigger(second = '*/1')
        ])
    ThreeSecond = OrTrigger([
        CronTrigger(second = '*/3')
        ])
    FiveSecond = OrTrigger([
        CronTrigger(second = '*/5')
        ])
    TenSecond = OrTrigger([
        CronTrigger(second = '*/10')
        ])
    TwentySecond = OrTrigger([
        CronTrigger(second = '*/20')
        ])
    OneMinute = OrTrigger([
        CronTrigger(minute = '*/1')
        ])
    # endregion
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
        while True:
            Connect_result = DataBase_Connect('127.0.0.1:27017',redundant = False)
            if not Connect_result: 
                print('MongoDB cannot connect!')
                sleep(2)
            else:
                break

        # 控制線程
        DO_control = Thread(target = EM_control)
        DO_control.start()
        # 門禁系統讀取線程
        AR = Thread(target = Access_Reading)
        AR.start()
        # 各副程式排程執行
        sched = BlockingScheduler({'apscheduler.job_defaults.max_instances':'50'})
        sched.add_job(Equipment_Status, FiveSecond)
        sched.add_job(Modbus_Device_Reading_1_1, FiveSecond)
        sched.add_job(Modbus_Device_Reading_1_2, FiveSecond)
        sched.add_job(Modbus_Device_Reading_1_3, OneSecond)
        sched.add_job(Modbus_Device_Reading_2, OneSecond)
        sched.add_job(Modbus_Device_Reading_3, OneSecond)
        sched.start()

    except Exception as meg:
        print('main error')
        print(meg)


if __name__=='__main__':
    main()

