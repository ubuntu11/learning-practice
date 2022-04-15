from pymongo import MongoClient


def connect_to_db(host, user='root', password='pc152', redundant=True):
    try:
        client = None
        if redundant:
            client = MongoClient(host, replicaset='rs0', serverSelectionTimeoutMS=10000)
        else:
            client = MongoClient(host, serverSelectionTimeoutMS=10000)
        # try:
        #     mongo_client_connect.admin.authenticate(user,password)
        # except ConnectionFailure as err_msg:
        #     count = count + 1
        #     print(str(err_msg) + '. {} times'.format(count))
        # else:
        #     break
        return client
    except Exception as mesg:
        print('DataBase_Connect error', mesg)
        return None


def insert_doc(db_client, collection, data, program_name, db ='AFC', num ='one'):
    try:
        if num == 'one':
            upload_status = db_client[db][collection].insert_one(data)
        elif num == 'many':
            upload_status = db_client[db][collection].insert_many(data)
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
