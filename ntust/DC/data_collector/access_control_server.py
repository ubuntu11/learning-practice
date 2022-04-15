from time import sleep
from datetime import datetime
import socket
from data_access import insert_doc


def start_server(db_client):
    try:
        HOST = '192.168.2.103'
        PORT = 5502
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
            if insert_doc(db_client, collection ='access', data = access , program_name ='Access_Reading'):
                print('write doc successfully')
            else:
                print('fail to write doc')

            conn.close()
        return True
    except Exception as meg:
        print('Access_Reading error')
        print(meg)
        conn.close()
        return False
