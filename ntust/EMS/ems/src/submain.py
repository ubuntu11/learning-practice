import pymongo
import datetime
from apscheduler.schedulers.background import BackgroundScheduler
import current as c
import os
import time
from dateutil.relativedelta import relativedelta
import psutil
import socket
from itertools import chain
from report import genReport


conn_self = pymongo.MongoClient('mongodb://user:pwd@127.0.0.1:27017/',serverSelectionTimeoutMS=10)
conn = pymongo.MongoClient('mongodb://127.0.0.1:27017/',serverSelectionTimeoutMS=10)
# conn = pymongo.MongoClient('mongodb://user:pwd@192.168.1.2:27017, 192.168.1.3:27017', replicaset='rs0', serverSelectionTimeoutMS=10)
# conn = pymongo.MongoClient(['192.168.1.2:27017','192.168.1.3:27017'],replicaset='rs0',serverSelectionTimeoutMS=10000)

db = conn['AFC']
_db = conn_self['admin']
#----------------------------------------------------------------------------------------------------------------------------------------
def live():
    db.wathcdog
#----------------------------------------------------------------------------------------------------------------------------------------

#def GetLocalIPByPrefix(prefix='192.168.0.100'):
def GetLocalIPByPrefix(prefix='127.0.0.1'):
    r"""
    多网卡情况下，根据前缀获取IP
    测试可用：Windows、Linux，Python 3.6.x，psutil 5.4.x
    ipv4/ipv6 地址均适用
    注意如果有多个相同前缀的 ip，只随机返回一个
    """
    localIP = 0
    dic = psutil.net_if_addrs()
    for adapter in dic:
        snicList = dic[adapter]
        for snic in snicList:
            if not snic.family.name.startswith('AF_INET'):
                continue
            ip = snic.address
            if ip.startswith(prefix):
                localIP = 1
    return localIP
#----------------------------------------------------------------------------------------------------------------------------------------
def status():
    if(GetLocalIPByPrefix()!=0):
        tStart1 = time.time()
        now = datetime.datetime.now()
        # try:
        db.program.update_many({},{'$inc':{'count':1}})
        db.equipment_status.update_many({},{'$inc':{'count':1}})
        warning = [] # 異常
        tonormal = [] #恢復
        for i in chain(db.program.find(),db.equipment_status.find()):
            ID = i["ID"]
            if(i["status"][0]==0 and i["count"]>i["limit"]): # 異常
                print("異常")
                event = i["name"]+"異常"
                level = i["level"]
                db.program.update_one({'_id':i['_id']},{'$set':{'status.0':1}})
                db.equipment_status.update_one({'_id':i['_id']},{'$set':{'status.0':1}})
                db.alarm.insert_one({'ID':ID,'time':now,'event':event,'level':level,'checktime':None,'returntime':None,"value":None,'show':1})
                c.SOE(db,ID,event,"status")
            elif(i["status"][0]==1 and i["count"]<i["limit"]): #恢復
                print("恢復")
                event = i["name"]+"異常"
                db.program.update_one({'_id':i['_id']},{'$set':{'status.0':0}})
                db.equipment_status.update_one({'_id':i['_id']},{'$set':{'status.0':0}})
                db.alarm.update_many({'ID':ID,'event':event},{'$set':{'returntime':now}})
                event = i["name"]+"恢復正常"
                c.SOE(db,ID,event,"status")
            else: # 正常
                pass
        # except:
        #     print('資料庫異常～～～～～～')
        #print('---------------------------------')
        tEnd1 = time.time()#計時結束
        #列印結果
        print("status cost %f sec" % (tEnd1 - tStart1))#會自動做近位
    else:
        print("status不在本機執行！！")
#----------------------------------------------------------------------------------------------------------------------------------------
def reset():
    db.program.update_one({},{'$set':{"status":0,'count':0}},multi=True)
    return False
#----------------------------------------------------------------------------------------------------------------------------------------
def info():
    now = datetime.datetime.now()
    # 查看cpu的信息
    cpu = psutil.cpu_percent(1)
    # 查看内存信息,剩餘内存.free  總共.total
    total = str(round(psutil.virtual_memory().total/(1024.0*1024.0*1024.0), 2))
    memory = int(psutil.virtual_memory().total-psutil.virtual_memory().available)/float(psutil.virtual_memory().total)
    #系统啟動时间
    boot_time =  datetime.datetime.fromtimestamp(psutil.boot_time())
    mongoState = None
    try:
        mongoState = _db.command("replSetGetStatus")['myState']
    except:
        pass
    try:
        name = socket.gethostname()
        print(GetLocalIPByPrefix(),'ssssssssssss')
        db.pc_info.insert_one({"ID":name,"time":now,'boot_time':boot_time,"cpu":cpu,"memory":{"total":total,"mem_per":memory},"IP":GetLocalIPByPrefix(),"mongoState":mongoState})
        print('---------------------------------------------------------')
        print('時間：',now)
        print("MongoDB狀態: %s" % mongoState)
        print("172.16.0.20: %s" % GetLocalIPByPrefix())
        print(u"系统啟動时间: %s" % boot_time.strftime("%Y-%m-%d %H:%M:%S"))
        print(u"cup使用率: %s %%" % cpu)
        print(u"物理内存： %s G" % total)
        print(u"記憶體使用率： %s %%" % int(memory*100))
        print('---------------------------------------------------------')
    except:
        print('event: info error')
#----------------------------------------------------------------------------------------------------------------------------------------
def accessToSOE():
    if(GetLocalIPByPrefix()!=0):
        for equip in db.equipment.find({"type":"access"}):
            model = db.model.find_one({"ID":equip["model"]})
            data = db.access.find_one({"ID":str(equip["_id"]),"read": {'$in': [None, False]}})
            # data = c.current_data(db,"access",str(equip["_id"]))[0]
            # print(data,"SSSSSSSSSSSSSSSSSSSSSS")
            if(model != None and data != None):
                tags = model["tags"]
                event = "時間:"+data["time"].strftime("%Y-%m-%d %H:%M:%S")+', '
                length = len(tags)
                for index,tag in enumerate(tags) :
                    event+=tag["name"]+":"+data[tag["tag"]]
                    if(index!=length-1):
                        event+=", "
                db.access.update_one({"_id":data["_id"]},{"$set":{"read":1}})
                c.SOE(db,data["ID"],event,"access")
    else:
        print("accessToSOE不在本機執行！！")
#----------------------------------------------------------------------------------------------------------------------------------------
alarm_equip_table = {}
def alarm_detector():
    if(GetLocalIPByPrefix()!=0):
        tStart1 = time.time()#計時開始
        now = datetime.datetime.now()
        #print('--------------Start--------------')
        protection = 0
        for equipID in alarm_equip_table:
            equip = alarm_equip_table[equipID]
            col = db[equip["type"]]
            data = col.find_one({"ID":equip["ID"]},{"time":1,**equip["tagsDict"]},sort=[("time",-1)])
            # print(data)
            if(data != None):
                db.alarm.update_many({"ID":equip["ID"],"returntime":None,"show":{"$gt":0},"count":1},{"$set":{"count":0}})
                for tag in equip["tags"]:
                    if(tag["tagType"] == "bits"):
                        if tag["tag"] in data:
                            try:
                                alarmData = format(int(data[tag["tag"]]),'016b')
                            except:
                                alarmData = format(0,'016b')
                            for idx,i in enumerate(alarmData):
                                _idx = (idx - 15) * -1
                                if(i=='1'):
                                    # print(tag["tag"],_idx,i)
                                    sameIndex = next((index for index,bit in enumerate(tag['bits']) if bit["index"] == _idx), None)
                                    if(sameIndex!=None):
                                        alarm = tag['bits'][sameIndex].get("alarm",[])
                                        if(len(alarm)>0):
                                            # print(alarm[0])
                                            event = tag["name"]+":"+alarm[0]["event"]
                                            level = alarm[0]['level']
                                            protection = max(alarm[0].get("protection",0), protection)
                                            # print(alarm[0])
                                            print(event,"protection:",alarm[0].get("protection",0))
                                            if (db.alarm.find_one({"ID":equip["ID"],"returntime":None,"event":event,"show":{"$gt":0}},{})== None):#發生異常
                                                db.alarm.insert_one({
                                                    "ID":equip["ID"],
                                                    "time":now,
                                                    "event":event,
                                                    "level":level,
                                                    "checktime":None,
                                                    "returntime":None,
                                                    "value":None,
                                                    "show":1,
                                                    "count":1
                                                })
                                            else: #持續異常
                                                db.alarm.update_one({"ID":equip["ID"],"returntime":None,"event":event,"show":{"$gt":0},"count":0},{"$set":{"count":1}})
                                                # print("持續異常")
                    elif(tag["tagType"] == "bitsArray"):
                        if tag["tag"] in data:
                            for index,i in enumerate(data[tag["tag"]]):
                                try:
                                    alarmData = format(int(i),'016b')
                                except:
                                    alarmData = format(0,'016b')
                                for idx,i in enumerate(alarmData):
                                    _idx = (idx - 15) * -1
                                    if(i=='1'):
                                        # print(tag["tag"],_idx,i)
                                        sameIndex = next((index for index,bit in enumerate(tag['bits']) if bit["index"] == _idx), None)
                                        if(sameIndex!=None):
                                            alarm = tag['bits'][sameIndex].get("alarm",[])
                                            if(len(alarm)>0):
                                                # print(alarm[0])
                                                event = tag["name"]+":"+alarm[0]["event"]+" (""Index:"+str(index+1)+")"
                                                level = alarm[0]['level']
                                                protection = max(alarm[0].get("protection",0), protection)
                                                print(event,"protection:",alarm[0].get("protection",0))
                                                if (db.alarm.find_one({"ID":equip["ID"],"returntime":None,"event":event,"show":{"$gt":0}},{})== None):#發生異常
                                                    db.alarm.insert_one({
                                                        "ID":equip["ID"],
                                                        "time":now,
                                                        "event":event,
                                                        "level":level,
                                                        "checktime":None,
                                                        "returntime":None,
                                                        "value":None,
                                                        "show":1,
                                                        "count":1
                                                    })
                                                else: #持續異常
                                                    db.alarm.update_one({"ID":equip["ID"],"returntime":None,"event":event,"show":{"$gt":0},"count":0},{"$set":{"count":1}})
                                                    # print("持續異常")
                    elif(tag["tagType"] == "bitsArray2"):
                        if tag["tag"] in data:
                            for i_index,i in enumerate(data[tag["tag"]]):
                                for j_index,j in enumerate(i):
                                    try:
                                        alarmData = format(int(j),'08b')
                                    except:
                                        alarmData = format(0,'08b')
                                    #print(alarmData)
                                    for idx,k in enumerate(alarmData):
                                        _idx = (idx - 7) * -1
                                        if(k=='1'):
                                            # print(tag["tag"],_idx,i)
                                            sameIndex = next((index for index,bit in enumerate(tag['bits']) if bit["index"] == _idx), None)
                                            # print(sameIndex)
                                            if(sameIndex!=None):
                                                alarm = tag['bits'][sameIndex].get("alarm",[])
                                                # print(alarm)
                                                if(len(alarm)>0):
                                                    # print(alarm[0])
                                                    event = tag["name"]+":"+alarm[0]["event"]+" (""Index:["+str(i_index+1)+","+str(j_index+1)+"])"
                                                    level = alarm[0]['level']
                                                    protection = max(alarm[0].get("protection",0), protection)
                                                    print(event,"protection:",alarm[0].get("protection",0))
                                                    if (db.alarm.find_one({"ID":equip["ID"],"returntime":None,"event":event,"show":{"$gt":0}},{})== None):#發生異常
                                                        db.alarm.insert_one({
                                                            "ID":equip["ID"],
                                                            "time":now,
                                                            "event":event,
                                                            "level":level,
                                                            "checktime":None,
                                                            "returntime":None,
                                                            "value":None,
                                                            "show":1,
                                                            "count":1
                                                        })
                                                    else: #持續異常
                                                        db.alarm.update_one({"ID":equip["ID"],"returntime":None,"event":event,"show":{"$gt":0},"count":0},{"$set":{"count":1}})
                                                        # print("持續異常")
                    elif(tag["tagType"] == "number" or tag["tagType"] == "text"):
                        if(tag["tag"]  in data):
                            alarmData = data[tag["tag"]]
                            if alarmData != None:
                                for alarm in tag["alarm"]:
                                    alarmFlag = 0
                                    if(alarm["operate"] == '='):
                                        alarmFlag = 1 if alarmData == alarm["value"] else 0
                                    elif(alarm["operate"] == '>'):
                                        alarmFlag = 1 if alarmData > alarm["value"] else 0
                                    elif(alarm["operate"] == '>='):
                                        alarmFlag = 1 if alarmData >= alarm["value"] else 0
                                    elif(alarm["operate"] == '<'):
                                        alarmFlag = 1 if alarmData > alarm["value"] else 0
                                    elif(alarm["operate"] == '<='):
                                        alarmFlag = 1 if alarmData <= alarm["value"] else 0

                                    if(alarmFlag == 1):
                                        alarmData = None if tag["tagType"] == "text" else alarmData
                                        event = tag["name"]+":"+alarm["event"]
                                        level = alarm['level']
                                        protection = max(alarm.get("protection",0), protection)
                                        # print("SSSSSSS")
                                        print(event,"protection:",alarm.get("protection",0))
                                        if (db.alarm.find_one({"ID":equip["ID"],"returntime":None,"event":event,"show":{"$gt":0}},{})== None):#發生異常
                                            #print("insert","code:"+str(i))
                                            db.alarm.insert_one({
                                                "ID":equip["ID"],
                                                "time":now,
                                                "event":event,
                                                "level":level,
                                                "checktime":None,
                                                "returntime":None,
                                                "value":alarmData,
                                                "show":1,
                                                "count":1
                                            })
                                        else: #持續異常
                                            db.alarm.update_one({"ID":equip["ID"],"returntime":None,"event":event,"show":{"$gt":0},"count":0},{"$set":{"value":alarmData,"count":1}})

                db.alarm.update_many({"ID":equip["ID"],"returntime":None,"show":{"$gt":0},"count":0},{"$set":{"returntime":now,"count":-1,"show":2}})
        #print('---------------------------------')
        protect(protection)
        tEnd1 = time.time()#計時結束
        #列印結果
        print("alarm_detector cost %f sec" % (tEnd1 - tStart1))#會自動做近位
    else:
        print("alarm_detector不在本機執行！！")
if True:
    for equip in db.equipment.find():
        equip['ID'] = str(equip['_id'])
        equip.pop('_id')
        modeltag = db.model.find_one({"ID":equip["model"],"type":equip["type"]},{"tags":1})
        # print(modeltag)
        alarm = 0
        if modeltag != None:
            modeltag = modeltag.get("tags",[])
            alarmtag = []
            alarmtagDict = {}
            for tag in modeltag:
                if(tag["tagType"]=="number" or tag["tagType"]=="numberArray" or tag["tagType"]=="text" or tag["tagType"]=="string" ):
                    alarmList = tag.get("alarm",[])
                    if(len(alarmList)>0):
                        alarm = 1
                        alarmtag.append(tag)
                        alarmtagDict[tag["tag"]] = 1
                elif(tag["tagType"]=="bits" or tag["tagType"]=="bitsArray" or tag["tagType"]=="bitsArray2"):
                    for bit in tag["bits"]:
                        alarmList = bit.get("alarm",[])
                        if(len(alarmList)>0):
                            alarm = 1
                            alarmtag.append(tag)
                            alarmtagDict[tag["tag"]] = 1
                            break
        if(alarm == 1):
            alarm_equip_table[equip["ID"]] = {**equip,"tags":alarmtag,"tagsDict":alarmtagDict}
#----------------------------------------------------------------------------------------------------------------------------------------
pcsIDList = []
mbmsIDList = []
airIDList = []
protection_reg = 0
protect_time = None
def protect(protection=0):
    global protection_reg, protect_time
    # protection 目前僅有關閉PCS功能
    # 關閉電池relay功能暫無完整，需加入PCS的狀態點位
    # return False  # ***暫時將保護功能擋掉
    if protection>protection_reg and protection_reg!=-1:
        protection_reg = protection
    now = datetime.datetime.now()
    old_sys_set = c.current_data(db,'sys_control','system')[0]
    # try:
    #     protect_time = db.sys_protect_log.find_one({"ID":"system","returntime":None},{"time":1}).get("time",None)
    # except:
    #     protect_time = None
    print("protection:",protection)
    # if protection == 1 or (pcs_operation_mode != 0 and protection == 2):
    if(protection == 1 or protection_reg == 1 or  ((protection >= 2 or protection_reg >= 2) and protect_time == None) ):
        if(db.sys_protect_log.find_one({"ID":"system","returntime":None}) == None):
            db.sys_protect_log.insert_one({"ID":"system","time":now,"returntime":None,"protect":protection})
            protect_time = now
            # air condition stop
            if protection == 3:
                # protection flag
                if(old_sys_set=={}):
                    old_sys_set = {"ID":"system","time":now,"protection":3}
                    db.sys_control.insert_one(old_sys_set)
                else:
                    old_sys_set.update_one({"protection":3,"time":now})
                    if "_id" in old_sys_set:
                        del old_sys_set["_id"]
                    db.sys_control.insert_one(old_sys_set)
                for i in airIDList:
                    db.air_control.insert_one({"ID":i,"time":now,"control":0,"reset":0})
                c.SOE(db,"system","保護邏輯-Step0.冷氣設定關閉","protection")
            # protection flag
            if(old_sys_set=={}):
                old_sys_set = {"ID":"system","time":now,"protection":1}
                db.sys_control.insert_one(old_sys_set)
            else:
                old_sys_set.update_one({"protection":1,"time":now})
                if "_id" in old_sys_set:
                    del old_sys_set["_id"]
                db.sys_control.insert_one(old_sys_set)
            # mode = 0 stop
            old_mode_set = c.current_data(db,'site_control','site_control')[0]
            if(old_mode_set=={}):
                old_mode_set = {"ID":"site_control","mode":0}
            old_mode_set.update_one({"mode":0,"time":now})
            db.site_control.insert_one(old_mode_set)
            c.SOE(db,"system","保護邏輯-Step1.模式設定為停止模式","protection")
            # pcs stop
            # db.pcs_mode.insert_one(old_mode_set)
            for i in pcsIDList:
                db.pcs_control.insert_one({"ID":i,"time":now,"control":0,"reset":0})
            # now = now + relativedelta(microseconds=+100000)
            c.SOE(db,"system","保護邏輯-Step2.PCS設定關閉,解除PCS搭接狀態","protection")
            if protection_reg==1:
                protection_reg=-1
            print('protect step')
        else:
            print('now protect1')
        print("protect end")
    elif(protection >= 2 or protection_reg >= 2):
        # print("protection time:",now, protect_time)
        if(now > protect_time+relativedelta(seconds=+5)):
            pcsStop = True
            for pcsID in pcsIDList:
                data = c.current_data(db,'pcs',pcsID,errortime=5)[0]
                if bool(data):
                    if data["control_state2"]==1 or  data["control_state2"]==2:
                        pcsStop &= True
                    else:
                        pcsStop &= False
                else:
                    pcsStop &= False
            # print(pcsStop)
            if(now > protect_time+relativedelta(seconds=+10)) or pcsStop:
                if(db.sys_protect_log.find_one({"ID":"system","returntime":None,"protect":{'$gte':2}}) == None or protection_reg >= 2):
                    if(old_sys_set=={}):
                        old_sys_set = {"ID":"system","time":now,"protection":2}
                        db.sys_control.insert_one(old_sys_set)
                    else:
                        old_sys_set.update_one({"protection":2,"time":now})
                        if "_id" in old_sys_set:
                            del old_sys_set["_id"]
                        db.sys_control.insert_one(old_sys_set)
                    # db.sys_protect_log.update_one({"ID":"system","returntime":None},{"$set":{"protect":2}})
                    # print("#################################battery open!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
                    for i in mbmsIDList:
                        db.mbms_control.insert_one({"ID":i,"time":now,"control":0,"reset":0})
                    c.SOE(db,"system","保護邏輯-Step3.電池組設定切離,解除Realy搭接狀態","protection")
                    protection_reg=-1
                else:
                    print('now protect2')
                print("protect end")
    else:
        protection_reg = 0
        protect_time = None
        if(db.sys_protect_log.find_one({"ID":"system","returntime":None}) != None):
            c.SOE(db,"system","保護邏輯解除","protection")

        db.sys_protect_log.update_one({"ID":"system","returntime":None},{"$set":{"returntime":now}})
        if old_sys_set["protection"] == 1 or old_sys_set["protection"] >= 2:
            old_sys_set.update_one({"protection":0,"time":now})
            db.sys_control.insert_one(old_sys_set)
        print("normal")
if True:
    for equip in db.equipment.find({"type":"pcs"}):
        pcsIDList.append(str(equip['_id']))
    for equip in db.equipment.find({"type":"mbms"}):
        mbmsIDList.append(str(equip['_id']))
    for equip in db.equipment.find({"type":"air"}):
        airIDList.append(str(equip['_id']))
#-------------------------------------------------------------------------------------------
if __name__ == '__main__':
    print('---------------------------------------------------------')
    print('Press Ctrl+{0} to exit'.format('Break' if os.name == 'nt' else 'C'))
    print('---------------------------------------------------------')
    scheduler = BackgroundScheduler()
    scheduler.start()
    # 狀態檢查
    scheduler.add_job(status,'cron', hour='*',minute='*',second='*')
    # 電腦資訊監測程序
    scheduler.add_job(info,'cron', hour='*',minute='*',second='*/5')
    # 門禁記錄程序
    scheduler.add_job(accessToSOE,'cron', hour='*',minute='*',second='*')
    # alarm偵測
    scheduler.add_job(alarm_detector,'cron', hour='*',minute='*',second='*')

    #月報
    scheduler.add_job(genReport,'cron', day='1',hour='1',minute='0',second='1' ,args=["month"])
    #季報
    scheduler.add_job(genReport,'cron', month="1,4,7,10",day='1',hour='1',minute='0',second='1',args=["quarter"])
    #年報
    scheduler.add_job(genReport,'cron', month="1",day='1',hour='1',minute='0',second='1',args=["year"])
    #報表測試
    #scheduler.add_job(genReport,'cron',minute='*/5',second='1' ,args=["quarter"])


    try:
    # This is here to simulate application activity (which keeps the main thread alive).
        while True:
            time.sleep(1)
    except (KeyboardInterrupt, SystemExit):
        # Not strictly necessary if daemonic mode is enabled but should be done if possible
        scheduler.shutdown()
        print('Exit The Job!')
