from math import isnan
from struct import unpack


def bytes_array_to_float32(value_high, value_low):
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


def bytes_array_to_uint(value_array, point_data):
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


def bytes_array_to_int(value_array, point_data):
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
