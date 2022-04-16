from datetime import datetime
from pyModbusTCP.client import ModbusClient
from data_access import insert_doc
from data_converter import bytes_array_to_uint, bytes_array_to_int
from model import equipment, PM_Data, TMM70, LM_Data, S42


def read_meters(db_client, aux_meter_ip, aux_meter_port, lv_meter_ip, lv_meter_port):
    """
    讀取輔電與低壓側電表數據並寫入EMS MongoDB.
    :param lv_meter_port:
    :param lv_meter_ip:
    :param aux_meter_port:
    :param aux_meter_ip:
    :param db_client: MongoClient instance
    :return: None
    """
    try:
        now_time = datetime.now()
        count = 0
        aux_meter = ModbusClient(host=aux_meter_ip, port=int(aux_meter_port), unit_id=1, timeout=1, auto_open=True,
                                 auto_close=True)
        lv_meter = ModbusClient(host=lv_meter_ip, port=int(lv_meter_port), unit_id=2, timeout=1, auto_open=True,
                                auto_close=True)

        result = [[], []]
        while True:
            result[0] = aux_meter.read_holding_registers(TMM70['read_data']['start'][0],
                                                         TMM70['read_data']['length'][0])
            result[1] = aux_meter.read_holding_registers(TMM70['read_data']['start'][1],
                                                         TMM70['read_data']['length'][1])
            # print (result[0])
            # print (result[1])
            if (result[0] != None) & (result[1] != None):
                equipment['輔助電源電表'] = 1
                if '_id' in PM_Data:
                    del PM_Data['_id']
                    del PM_Data['time']
                for key in PM_Data.keys():
                    # print (key)
                    if (key != 'ID') & (key != 'time'):
                        if TMM70[key]['order'] != None:
                            if TMM70[key]['type'] == 'uint':
                                PM_Data[key] = bytes_array_to_uint(result[TMM70[key]['order']], TMM70[key])
                            elif TMM70[key]['type'] == 'int':
                                PM_Data[key] = bytes_array_to_int(result[TMM70[key]['order']], TMM70[key])
                PM_Data['time'] = now_time
                # ---------------------------test_start------------------------------
                # print(PM_Data)
                # for key in PM_Data.keys():
                #     print (key+' :'+str(PM_Data[key]))
                # ---------------------------test_end--------------------------------
                insert_doc(db_client, collection='acm', data=PM_Data, program_name='Modbus_Device_Reading_1_1')
                break
            else:
                count += 1
                print("Power Meter reading fail {} times".format(count))
                if count >= 1:
                    equipment['輔助電源電表'] = 0
                    count = 0
                    break
        # endregion
        # ------------------------------------------------------------------------------------------------
        # region Power Meter Reading
        result = [[], []]
        while True:
            result[0] = lv_meter.read_holding_registers(TMM70['read_data']['start'][0], TMM70['read_data']['length'][0])
            result[1] = lv_meter.read_holding_registers(TMM70['read_data']['start'][1], TMM70['read_data']['length'][1])
            # print (result[0])
            # print (result[1])
            if (result[0] != None) & (result[1] != None):
                equipment['低壓側電表'] = 1
                if '_id' in LM_Data:
                    del LM_Data['_id']
                    del LM_Data['time']
                for key in LM_Data.keys():
                    if (key != 'ID') & (key != 'time'):
                        if TMM70[key]['order'] != None:
                            if TMM70[key]['type'] == 'uint':
                                LM_Data[key] = bytes_array_to_uint(result[TMM70[key]['order']], TMM70[key])
                            elif TMM70[key]['type'] == 'int':
                                LM_Data[key] = bytes_array_to_int(result[TMM70[key]['order']], TMM70[key])
                LM_Data['time'] = now_time
                # ---------------------------test_start------------------------------
                # print(LM_Data)
                # for key in LM_Data.keys():
                #     print (key+' :'+str(LM_Data[key]))
                # ---------------------------test_end--------------------------------
                insert_doc(db_client, collection='acm', data=LM_Data, program_name='Modbus_Device_Reading_1_1')
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
        print('read_meters error')
        print(meg)
        return False


def read_env(db_client, env_modbus_ip, env_modbus_port):
    """
    讀取環境資訊.
    :param env_modbus_port:
    :param env_modbus_ip:
    :param db_client: MongoClient instance
    :return: None
    """
    try:
        now_time = datetime.now()
        count = 0
        data = [{'ID': '60f8eaf254841d42146b3af5', 'temp': 0},
                ]
        mt6200 = ModbusClient(host=env_modbus_ip, port=int(env_modbus_port), unit_id=1, timeout=0.1, auto_open=True, auto_close=True)

        while True:
            result = mt6200.read_input_registers(0x1000, 1)
            if result != None:

                equipment['環境數據'] = 1
                data[0]['temp'] = result[0]
                data[0]['time'] = now_time
                insert_doc(db_client, collection='env', data=data, num='many', program_name='Modbus_Device_Reading_1_2')
                break
            else:
                count += 1
                print('MT6200 reading fail: {} times'.format(count))
                if count >= 4:
                    equipment['環境數據'] = 0
                    count = 0
                    break
        return True
    except Exception as meg:
        print('read_env error')
        print(meg)
        return False


def read_rio1(db_client, rio1_modbus_ip, rio1_modbus_port):
    """
    讀取第一台RIO.
    :param rio1_modbus_port:
    :param rio1_modbus_ip:
    :param db_client: MongoClient instance
    :return: None
    """
    try:
        now_time = datetime.now()
        count = 0
        data = [{'ID': '60f8eaf254841d42146b3aff', 'acb': 0, 'vcb': 0},
                ]

        adam4055 = ModbusClient(host=rio1_modbus_ip, port=int(rio1_modbus_port), unit_id=3, timeout=0.1, auto_open=True,
                                auto_close=True)
        while True:
            result = adam4055.read_holding_registers(0x0000, 1)
            # print (result)
            if result != None:
                byte_data = result[0]
                try:
                    binlist = list(bin(byte_data))
                    del binlist[0:2]
                    binlistint = list(map(lambda x: int(x), binlist))
                    while True:
                        if len(binlistint) < 16:
                            binlistint.insert(0, 0)
                        else:
                            break
                    binlistint.reverse()
                except Exception as meg:
                    print('ADAM4055 error')
                    print(meg)
                    break
                # print(binlistint)
                equipment['DI2'] = 1
                if (binlistint[0] == 1) & (binlistint[1] == 0) & (binlistint[2] == 0):
                    data[0]['acb'] = 0
                elif (binlistint[0] == 0) & (binlistint[1] == 1) & (binlistint[2] == 0):
                    data[0]['acb'] = 1
                elif (binlistint[2] == 1):
                    data[0]['acb'] = 2
                elif (binlistint[0] == 0) & (binlistint[1] == 0) & (binlistint[2] == 0):
                    data[0]['acb'] = 0
                elif (binlistint[0] == 1) & (binlistint[1] == 1) & (binlistint[2] == 0):
                    data[0]['acb'] = 1
                else:
                    data[0]['acb'] = 2

                if (binlistint[3] == 1) & (binlistint[4] == 0) & (binlistint[5] == 0):
                    data[0]['vcb'] = 0
                elif (binlistint[3] == 0) & (binlistint[4] == 1) & (binlistint[5] == 0):
                    data[0]['vcb'] = 1
                elif (binlistint[5] == 1):
                    data[0]['vcb'] = 2
                elif (binlistint[3] == 0) & (binlistint[4] == 0) & (binlistint[5] == 0):
                    data[0]['vcb'] = 0
                elif (binlistint[3] == 1) & (binlistint[4] == 1) & (binlistint[5] == 0):
                    data[0]['vcb'] = 1
                else:
                    data[0]['vcb'] = 2
                data[0]['time'] = now_time
                insert_doc(db_client, collection='rio', data=data, num='many', program_name='Modbus_Device_Reading_1_3')
                break
            else:
                count += 1
                print('ADAM4055 reading fail: {} times'.format(count))
                if count >= 4:
                    equipment['DI2'] = 0
                    count = 0
                    break
        return True
    except Exception as meg:
        print('read_rio1 error')
        print(meg)
        return False


def read_rio2(db_client, rio2_modbus_ip, rio2_modbus_port):
    """
    讀取第二台RIO.
    :param rio2_modbus_port:
    :param rio2_modbus_ip:
    :param db_client: MongoClient instance
    :return: None
    """
    try:
        now_time = datetime.now()
        count = 0
        data = [{'ID': '60f8eaf254841d42146b3afc',
                 'ready': 0,
                 'comm_alarm': 0,
                 'warning': 0,
                 'fault': 0,
                 'sp_ac_input_mccb_trip': 0,
                 'em_stop_out': 0, },
                ]
        adam6251 = ModbusClient(host=rio2_modbus_ip, port=int(rio2_modbus_port), unit_id=1, timeout=0.1, auto_open=True,
                                auto_close=True)

        while True:
            result = adam6251.read_holding_registers(0x0000, 1)
            if result != None:
                byte_data = result[0]
                try:
                    binlist = list(bin(byte_data))
                    del binlist[0:2]
                    binlistint = list(map(lambda x: int(x), binlist))
                    while True:
                        if len(binlistint) < 16:
                            binlistint.insert(0, 0)
                        else:
                            break
                    binlistint.reverse()
                except Exception as meg:
                    print('ADAM6251 error')
                    print(meg)
                    break
                # print (binlistint)
                equipment['DI1'] = 1
                if (binlistint[0] == 1):
                    data[0]['ready'] = 1
                else:
                    data[0]['ready'] = 0
                if (binlistint[1] == 1):
                    data[0]['comm_alarm'] = 1
                else:
                    data[0]['comm_alarm'] = 0
                if (binlistint[2] == 1):
                    data[0]['warning'] = 1
                else:
                    data[0]['warning'] = 0
                if (binlistint[3] == 1):
                    data[0]['fault'] = 1
                else:
                    data[0]['fault'] = 0
                if (binlistint[4] == 1):
                    data[0]['sp_ac_input_mccb_trip'] = 1
                else:
                    data[0]['sp_ac_input_mccb_trip'] = 0
                if (binlistint[5] == 1):
                    data[0]['em_stop_out'] = 1
                else:
                    data[0]['em_stop_out'] = 0
                data[0]['time'] = now_time
                insert_doc(db_client, collection='rio', data=data, num='many', program_name='Modbus_Device_Reading_2')
                break
            else:
                count += 1
                print('ADAM6251 reading fail: {} times'.format(count))
                if count >= 4:
                    equipment['DI1'] = 0
                    count = 0
                    break
        return True
    except Exception as meg:
        print('read_rio2 error')
        print(meg)
        return False


def read_relay(db_client, relay_modbus_ip, relay_modbus_port):
    """
    讀取保護電驛.
    :param relay_modbus_port:
    :param relay_modbus_ip:
    :param db_client: MongoClient instance
    :return: None
    """
    count = 0
    now_time = datetime.now()
    data = [{'ID': '60f8eba954841d42146b3cf8',
             'protection_50_51_1': 0,
             'protection_50_51_2': 0,
             'protection_50_51_3': 0,
             'protection_50_51_4': 0,
             'protection_50N_51N_1': 0,
             'protection_50N_51N_2': 0,
             'protection_50N_51N_3': 0,
             'protection_50N_51N_4': 0,
             'protection_81H_1': 0,
             'protection_81H_2': 0,
             'protection_81L_1': 0,
             'protection_81L_2': 0,
             'protection_81L_3': 0,
             'protection_81L_4': 0,
             'protection_27_27S_1': 0,
             'protection_27_27S_2': 0,
             'protection_67_1': 0,
             'protection_67_2': 0,
             'protection_59_1': 0,
             'protection_59_2': 0,
             'protection_59N_1': 0,
             'protection_59N_2': 0,
             }
            ]

    try:
        s42_tcp = ModbusClient(host=relay_modbus_ip, port=int(relay_modbus_port), unit_id=1, timeout=0.1, auto_open=True,
                               auto_close=True)
        result = [[], [], []]
        while True:
            result[0] = s42_tcp.read_holding_registers(S42['read_data']['start'][0], S42['read_data']['length'][0])
            result[1] = s42_tcp.read_holding_registers(S42['read_data']['start'][1], S42['read_data']['length'][1])
            result[2] = s42_tcp.read_holding_registers(S42['read_data']['start'][2], S42['read_data']['length'][2])

            if (result[0] != None) and (result[1] != None) and (result[2] != None):
                equipment['保護電驛'] = 1
                for key in data[0].keys():
                    if (key != 'ID') & (key != 'time'):
                        if S42[key]['order'] != None:
                            if result[S42[key]['order']][S42[key]['return_address']] == 1:
                                data[0][key] = 1
                            else:
                                data[0][key] = 0
                data[0]['time'] = now_time
                insert_doc(db_client, collection='protective_relay', data=data, num='many',
                           program_name='Modbus_Device_Reading_3')
                break
            else:
                count += 1
                print('SEPAMS42 relay reading fail: {} times'.format(count))
                if count >= 4:
                    equipment['保護電驛'] = 0
                    count = 0
                    break
    except Exception as meg:
        print('read_relay error')
        print(meg)
        return False
