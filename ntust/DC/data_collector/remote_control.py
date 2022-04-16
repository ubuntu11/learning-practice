from time import sleep
from pyModbusTCP.client import ModbusClient
from model import equipment


def em_control(db_client, modbus_ip, modbus_port):
    em_status=10
    count=0
    adam6256 = ModbusClient(host = modbus_ip ,port = int(modbus_port) ,unit_id = 1 ,timeout = 0.1 ,auto_open = True ,auto_close = True)

    while True:
        try:
            #------------------------------------------------------------------------------------------------
            for DB_Result in db_client['AFC']['em_control'].find({'ID':'6055e92d7445a0a1cb1bb42c'}).sort('time' ,-1).limit(1):
                if int(DB_Result['control']) != em_status:
                    while True:
                        if DB_Result['control'] == 1:
                            write_data = 0x0000
                            write_check = True
                        else:
                            write_data = 0x0001
                            write_check = False
                        if adam6256.write_single_register(0x0000 ,write_data):
                            result = adam6256.read_holding_registers(0x0000 ,1)
                            print(result)
                            if (not result[0]) == write_check:
                                equipment['EM_STOP'] = 1
                                em_status = DB_Result['control']
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
                        result = adam6256.read_holding_registers(0x0000 ,1)
                        if result is not None:
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
