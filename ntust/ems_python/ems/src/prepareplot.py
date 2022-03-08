import pymongo
import datetime
import time 
from dateutil.relativedelta import relativedelta
from bson.objectid import ObjectId
from statistics import mean
import pandas as pd 
import numpy as np
#----------------------------------------------------------------------------------------zuaibing
def month_len(date):
    month_len = (datetime.datetime.combine(date, datetime.time.min) + relativedelta(day=1, months=+1, days=-1)).day
    return month_len
#----------------------------------------------------------------------------------------zuaibing
def t_x_axis(date=datetime.date.today()):
    global x_axis_1 
    x_axis_1 = []
    nexttime = datetime.datetime.combine(date, datetime.time.min)
    for i in range(0,86400):
        x_axis_1.append(datetime.datetime.strftime(nexttime,"%Y-%m-%d %H:%M:%S"))
        nexttime  = nexttime + datetime.timedelta(seconds=1) 
    global x_axis_2
    x_axis_2 = []
    nexttime = datetime.datetime.combine(date, datetime.time.min)
    for i in range(0,1440):
        x_axis_2.append(datetime.datetime.strftime(nexttime,"%Y-%m-%d %H:%M:%S"))
        nexttime  = nexttime + datetime.timedelta(seconds=60)  
    return False
#---------------------------------------------------------------------------------------
def fix_x_xis(date=datetime.date.today(),interval=1):
    x_axis =[]
   
    nexttime = datetime.datetime.combine(date, datetime.time.min)
    deltatime = datetime.timedelta(seconds=interval)

    for i in range(int(86400/interval)):
        x_axis.append(datetime.datetime.strftime(nexttime,"%Y-%m-%d %H:%M:%S"))
        nexttime  = nexttime + deltatime 
    return x_axis
#---------------------------------------------------------------------------------------
def time_x_xis(starttime,endtime,interval=1):
    # tStart1 = time.time()
    # x_axis = []
    # x_axis = np.array(pd.date_range(start=starttime, end=endtime, freq=str(interval)+"S")).tolist()
    x_axis = [d.strftime("%Y-%m-%d %H:%M:%S") for d in pd.date_range(start=starttime, end=endtime, freq=str(interval)+"S")]
    # x_axis = list(pd.date_range(start=starttime, end=endtime, freq=str(interval)+"S"))
    # tEnd1 = time.time()#計時結束
    # #列印結果
    # print("status cost %f sec" % (tEnd1 - tStart1),"SSSSSSSSSSSSSSSSSSSSSs")#會自動做近位  
    return x_axis[:-1]
#---------------------------------------------------------------------------------------
def interval_x_xis(start,num,fmt="%Y-%m-%d %H:%M:%S",interval=1):
    x_axis =[]
    nexttime = start
    deltatime = datetime.timedelta(seconds=interval)
    for i in range(int(num)):
        x_axis.append(datetime.datetime.strftime(nexttime,fmt))
        nexttime  = nexttime + deltatime 
    return x_axis
#------------------------------------------------------------------------------------
def before_x_xis(before=1,interval=1):
    x_axis =[]
    nexttime = datetime.datetime.now() - relativedelta( hours=before)
    deltatime = datetime.timedelta(seconds=interval)
    for i in range(int(before*3600/interval)):
        x_axis.append(datetime.datetime.strftime(nexttime,"%Y-%m-%d %H:%M:%S"))
        nexttime  = nexttime + deltatime 
    return x_axis
#------------------------------------------------------------------------------------
def date_interval(date=datetime.date.today()):
    starttime = datetime.datetime.combine(date, datetime.time.min)
    endtime = starttime+datetime.timedelta(days=1)
    return starttime,endtime
#------------------------------------------------------------------------------------
def week_interval(date=datetime.date.today()):
    starttime = datetime.datetime.combine(date, datetime.time.min)
    endtime = starttime+datetime.timedelta(days=7)
    return starttime,endtime
#------------------------------------------------------------------------------------
def month_interval(date=datetime.date.today()):
    starttime = datetime.datetime.combine(date + relativedelta(day=1), datetime.time.min)
    endtime = starttime+relativedelta(months=+1)
    return starttime,endtime
#------------------------------------------------------------------------------------
def year_interval(date=datetime.date.today()):
    starttime = datetime.datetime.combine(date + relativedelta(month=1,day=1), datetime.time.min)
    endtime = starttime+relativedelta(months=12,day=31)
    return starttime,endtime
#------------------------------------------------------------------------------------
def stopn(interval = 1):
    date=datetime.date.today()
    starttime = datetime.datetime.combine(date, datetime.time.min)
    localtime = datetime.datetime.now()
    stop_n = int((localtime - starttime).total_seconds()/interval)
    return stop_n
#---------------------------------------------------------------------------------------
def math_index(y_axis,layer = 1):
    _min = []
    _max = []
    _avg = []

    if(layer==1):
        mi = {}
        ma = {}
        avg = {}
        _y_axis = list(filter(Noneor0,y_axis))
        if(_y_axis!=[]):
            mi_num = min(_y_axis)
            mi['value'] = mi_num
            mi['time'] = y_axis.index(mi_num)
            ma_num = max(_y_axis)
            ma['value'] = ma_num
            ma['time'] = y_axis.index(ma_num)
            avg['value'] = mean(_y_axis)
            _min.append(mi)
            _max.append(ma)
            _avg.append(avg)
        else:
            mi['value'] = '---'
            mi['time'] = None
            ma['value'] = '---'
            ma['time'] = None
            avg['value'] = '---'
            _min.append(mi)
            _max.append(ma)
            _avg.append(avg)
    else:
        for i in range(layer):
            
            mi = {}
            ma = {}
            avg = {}
            _y_axis1 = y_axis[i]
            _y_axis = list(filter(Noneor0,_y_axis1))
            if(_y_axis!=[]):
                mi_num = min(_y_axis)
                mi['value'] = mi_num
                mi['time'] = _y_axis1.index(mi_num)
                ma_num = max(_y_axis)
                ma['value'] = ma_num
                ma['time'] = _y_axis1.index(ma_num)
                avg['value'] = mean(_y_axis)
                _min.append(mi)
                _max.append(ma)
                _avg.append(avg)
            else:
                mi['value'] = '---'
                mi['time'] = None
                ma['value'] = '---'
                ma['time'] = None
                avg['value'] = '---'
                _min.append(mi)
                _max.append(ma)
                _avg.append(avg)
    return _min,_max,_avg
#---------------------------------------------------------------------------------------
def Noneor0(x):
    if(x==0 or x==None):
        return False
    else:
        return True
#---------------------------------------------------------------------------------------
def dayline_one(db,collection,ID,datatype,date=datetime.date.today(),interval=1):
    db = db[collection]
    starttime,endtime = date_interval(date)#區間
    num = int((endtime-starttime).total_seconds()/interval)
    y = [None] * num
  
    #條件
    if ID !=None :
        match = {                                
            'ID':ID,
            'time': {
                '$gte': starttime,
                '$lte': endtime,
            }
        }
    else:
        match = {                                
            'time': {
                '$gte': starttime,
                '$lte': endtime,
            }
        }

    #群組
    group =  {
        '_id': ID,
        'time': {  '$push': "$time"  },
        datatype : { '$push': "$"+datatype  }
    }  
    #資料
    data = list(db.aggregate(
        [
            {'$match': match},
            {"$project":{"_id":0,'time':1,datatype:1}},  
            { '$limit' : 86400 },
            {'$group': group},
        ]
    ))
 
    if(data!=[]):
        for i,time in enumerate(data[0]['time']):
            diff = time - starttime 
            n = int(diff.total_seconds()/interval) 
            try: 
                y[n] = round(data[0][datatype][i],3)
            except:
                pass
    return y
#-------------------------------------------------------------------------------
def timeline_one(db,collection,ID,datatype,starttime,endtime,interval=1):
    db = db[collection]
    num = int((endtime-starttime).total_seconds()/interval)
    if(num>0):
        y = [None] * num
        print(num)
        #條件
        if ID !=None :
            match = {                                
                'ID':ID,
                'time': {
                    '$gte': starttime,
                    '$lt': endtime,
                }
            }
        else:
            match = {                                
                'time': {
                    '$gte': starttime,
                    '$lte': endtime,
                }
            }
        #群組
        group =  {
            '_id': ID,
            'time': {  '$push': "$time"  },
            datatype : { '$push': "$"+datatype  }
        }  
        #資料
        data = list(db.aggregate(
            [
                {'$match': match},
                {"$project":{"_id":0,'time':1,datatype:1}},  
                { '$limit' : num },
                {'$group': group},
            ]
        ))
        if(data!=[]):
            for i,time in enumerate(data[0]['time']):
                diff = time - starttime 
                n = int(diff.total_seconds()/interval) 
                try: 
                    y[n] = round(data[0][datatype][i],3)
                except:
                    pass
        return y
    else:
        return []
#-------------------------------------------------------------------------------
def timeline_one_array(db,collection,ID,datatype,starttime,endtime,name,length,rack_module,interval=1):
    db = db[collection]
    time_num = int((endtime-starttime).total_seconds()/interval)
    names = [] 
    for num in  range(1,length+1):
        names.append(name+str(num))
    if(time_num>0):
        y = [ [ None for i in range(time_num)] for j in range(length)]
        print(time_num)
        #條件
        if ID !=None :
            match = {                                
                'ID':ID,
                'time': {
                    '$gte': starttime,
                    '$lt': endtime,
                }
            }
        else:
            match = {                                
                'time': {
                    '$gte': starttime,
                    '$lte': endtime,
                }
            }
        #群組
        group =  {
            '_id': ID,
            'time': {  '$push': "$time"  }
        }  
        if(len(rack_module)==0):
            group[datatype] = { '$push': "$"+datatype }
        elif(len(rack_module)==1):
            group[datatype] = { '$push': {'$arrayElemAt':["$"+datatype, rack_module[0]]}  }
        else:
            group[datatype] = { '$push': {'$arrayElemAt':[{'$arrayElemAt':["$"+datatype, rack_module[0]]}, rack_module[1]]}  }
        #資料
        data = list(db.aggregate(
            [
                {'$match': match},
                {"$project":{"_id":0,'time':1,datatype:1}},  
                { '$limit' : time_num },
                {'$group': group},
            ],
            allowDiskUse=True
        ))
        if(data!=[]):
            for i,time in enumerate(data[0]['time']):
                diff = time - starttime 
                n = int(diff.total_seconds()/interval) 
                try: 
                    for num in  range(length):
                        # if(len(rack_module)==0):
                        y[num][n] = round(data[0][datatype][i][num],3)
                        # elif(len(rack_module)==1):
                        #     y[num][n] = round(data[0][datatype][i][rack_module[0]][num],3)
                        # else:
                        #     y[num][n] = round(data[0][datatype][i][rack_module[0]][rack_module[1]][num],3)
                except:
                    pass
        return y, names
    else:
        return []
#-------------------------------------------------------------------------------
def intervalline_x_y_more(db,collection,ID,datatype,before=1,interval=1):#before 幾小時前
    db = db[collection]
    endtime = datetime.datetime.now()
    starttime = endtime - relativedelta(hours=before)
    num = int((endtime-starttime).total_seconds()/interval)
    x =  before_x_xis(before,interval)
    y = [ [ None for i in range(num)] for j in range(len(datatype))]
    #條件
    if ID !=None :
        match = {                                
            'ID':ID,
            'time': {
                '$gte': starttime,
                '$lte': endtime,
            }
        }
    else:
        match = {                                
            'time': {
                '$gte': starttime,
                '$lte': endtime,
            }
        }

    #群組
    find = {}
    project = {"_id":0,'time':1}
    for i in datatype:
        find.update({ i:{ '$push': "$"+i  }})
        project.update({i:1})
    group =  {
        **{
            '_id': ID,
            'time': {  '$push': "$time"  },
        },
        **find
    }  
    #資料
    data = list(db.aggregate(
        [
            {'$match': match},
            {"$project":project},  
            { '$limit' : before*36000/interval },
            {'$group': group},
        ]
    ))
  
    if(data!=[]):
        for i,time in enumerate(data[0]['time']):
            diff = time - starttime 
            n = int(diff.total_seconds()/interval) 
            for j in range(len(datatype)):
                try: 
                    y[j][n] = round(data[0][datatype[j]][i],3)
                except:
                    pass
    return x,y
#--------------------------------------------------------------------------------
def dayline_one_update(db,collection,ID,datatype,date=datetime.date.today(),interval=1):
    db = db[collection]
    starttime,endtime = date_interval(date)#區間
    num = int((endtime-starttime).total_seconds()/interval)
    #條件
    match = {                                
        'ID':ID,
        'time': {
            '$gte': starttime,
            '$lte': endtime,
        }
    }
    #群組
    group =  {
        '_id': ID,
        'time': {  '$push': "$time"  },
        datatype : { '$push': "$"+datatype  }
    }  
    data = list(db.aggregate(
        [
            {'$match': match},         
            {"$project":{"_id":0,'time':1,datatype:1}},
            {'$sort': {'time':-1}}, 
            {'$limit': 30},
            {'$group': group},
        ]
    ))
    y= []
    if(data!=[]):   
        stop_n = 0
        start_n = 0 
        for i,time in enumerate(data[0]['time']):
            diff = time - starttime 
            n = int(diff.total_seconds()/interval) 
            if(i==0):
                stop_n = n
            else:
                start_n = n
            try: 
                y.append( [round(data[0][datatype][i],3),n] )
            except:
                pass
    return y
#----------------------------------------------------------------------
def dayline_three(db,collection,ID,datatype1,datatype2,datatype3,date=datetime.date.today(),interval=1):
    db = db[collection]
    starttime,endtime = date_interval(date)#區間
    num = int((endtime-starttime).total_seconds()/interval)
    print(interval)
    y = [ [ None for i in range(num)] for j in range(3)]
    
    #條件
    print(datatype1,datatype2,datatype3)
    match = {                                
        'ID':ID,
        'time': {
            '$gte': starttime,
            '$lte': endtime,
        }
    }
    #群組
    group =  {
        '_id': ID,
        'time': {  '$push': "$time"  },
        datatype1 : { '$push': "$"+datatype1  },
        datatype2 : { '$push': "$"+datatype2  },
        datatype3 : { '$push': "$"+datatype3  },
    }  
    #資料
    data = list(db.aggregate(
        [
            {'$match': match},
            {"$project": {"_id":0,'time':1,datatype1:1,datatype2:1,datatype3:1}},  
            {'$group': group},
        ]
    ))
    
    if(data!=[]):
        for i,time in enumerate(data[0]['time']):
            diff = time - starttime 
            n = int(diff.total_seconds()/interval) 
            try: 
                y[0][n] = data[0][datatype1][i]
            except:
                pass
            try: 
                y[1][n] = data[0][datatype2][i]
            except:
                pass
            try: 
                y[2][n] = data[0][datatype3][i]
            except:
                pass
    return y
#----------------------------------------------------------------------
def dayline_four(db,collection,ID,datatype1,datatype2,datatype3,datatype4,date=datetime.date.today(),interval=1):
    db = db[collection]
    starttime,endtime = date_interval(date)#區間
    num = int((endtime-starttime).total_seconds()/interval)
    print(interval)
    y = [ [ None for i in range(num)] for j in range(4)]
    
    #條件
    print(datatype1,datatype2,datatype3,datatype4)
    if(ID!=None):
        match = {                                
            'ID':ID,
            'time': {
                '$gte': starttime,
                '$lte': endtime,
            }
        }
    else:
        match = {                                
            'time': {
                '$gte': starttime,
                '$lte': endtime,
            }
        }
    #群組
    group =  {
        '_id': ID,
        'time': {  '$push': "$time"  },
        datatype1 : { '$push': "$"+datatype1  },
        datatype2 : { '$push': "$"+datatype2  },
        datatype3 : { '$push': "$"+datatype3  },
        datatype4 : { '$push': "$"+datatype4  },
    }  
    #資料
    data = list(db.aggregate(
        [
            {'$match': match},
            {"$project":{"_id":0,'time':1,datatype1:1,datatype2:1,datatype3:1,datatype4:1}},  
            {'$group': group},
        ]
    ))
    
    if(data!=[]):
        for i,time in enumerate(data[0]['time']):
            diff = time - starttime 
            n = int(diff.total_seconds()/interval) 
            try: 
                y[0][n] = data[0][datatype1][i]
            except:
                pass
            try: 
                y[1][n] = data[0][datatype2][i]
            except:
                pass
            try: 
                y[2][n] = data[0][datatype3][i]
            except:
                pass
            try: 
                y[3][n] = data[0][datatype4][i]
            except:
                pass
    return y
#----------------------------------------------------------------------
def timeline_multi(db,collection,ID,datatypes,starttime,endtime,interval=1):
    db = db[collection]
    num = int((endtime-starttime).total_seconds()/interval)
    print(interval)
    y = [ [ None for i in range(num)] for j in range(len(datatypes))]
    
    #條件
    match = {                                
        'ID':ID,
        'time': {
            '$gte': starttime,
            '$lt': endtime,
        }
    }
    #篩選
    project = {"_id":0, 'time':1}
    #群組
    group =  {
        '_id': ID,
        'time': {  '$push': "$time"  }
    }  
    for idx, datatype in enumerate(datatypes):
        if "." in datatype:
            keys = datatype.split(".")
            project[keys[0]] = 1
            group[keys[0]+keys[1]] = {'$push': {"$arrayElemAt":["$"+keys[0],int(keys[1])]}}
            datatypes[idx] = keys[0]+keys[1]
        else:
            project[datatype] = 1
            group[datatype] = {'$push': "$"+datatype}
    #資料
    # print(group)
    data = list(db.aggregate(
        [
            {'$match': match},
            {"$project": project},  
            {'$group': group},
        ]
    ))
    if(data!=[]):
        for i,time in enumerate(data[0]['time']):
            diff = time - starttime 
            n = int(diff.total_seconds()/interval) 
            for datatype_idx, datatype in enumerate(datatypes):
                try: 
                    # print("dddddddddddddddddddddddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadddddddddddddddddddddddddd",datatype)
                    if datatype=='second7':
                        y[datatype_idx][n] = data[0][datatype][i]/1000
                    else:
                        y[datatype_idx][n] = data[0][datatype][i]
                except:
                    pass
    return y
#-------------------------------------------------------------------------------
def bar_x_xis(starttime,endtime,date_range):
    x = []
    month = 0
    day = 0
    hour = 0
    fmt = ""
    if(date_range==1):
        month = 0
        day = 0
        hour = 1
        fmt = "%Y-%m-%d %H"
    elif(date_range==2):
        month = 0
        day = 1
        hour = 0
        fmt = "%Y-%m-%d"
    elif(date_range==3):
        month = 1
        day = 0
        hour = 0
        fmt = "%Y-%m"
    else:
        return x
    nexttime = starttime
    while(nexttime<endtime):
        nexttime = starttime + relativedelta(months=month,days=day,hours=hour)
        x.append(datetime.datetime.strftime(starttime, fmt) )
        starttime = nexttime
    return x
#-------------------------------------------------------------------------------
def bar_one(db,collection,ID,datatype,starttime,endtime,date_range):
    db = db[collection]
    y = []
    month = 0
    day = 0
    hour = 0
    if(date_range==1):
        month = 0
        day = 0
        hour = 1
    elif(date_range==2):
        month = 0
        day = 1
        hour = 0
    elif(date_range==3):
        month = 1
        day = 0
        hour = 0
    else:
        return y
    nexttime = starttime 
    while(nexttime<endtime):
        nexttime = starttime + relativedelta(months=month,days=day,hours=hour)
        maxData = db.find_one({"ID":ID,"time":{"$gte":starttime,"$lt":nexttime}},{"_id":0,datatype:1},sort=[( 'time', pymongo.DESCENDING )])
        minData = db.find_one({"ID":ID,"time":{"$gte":starttime,"$lt":nexttime}},{"_id":0,datatype:1},sort=[( 'time', pymongo.ASCENDING )])
        if(maxData != None and minData != None):
            try:
                y.append(maxData[datatype] - minData[datatype])
            except:
                y.append(None)
        else:
            y.append(None)
        starttime = nexttime
    return y
#---------------------------------------------------------------------------------------zuaibing
def hourline_one(db,collection,ID,datatype,date=datetime.datetime.now(),interval=0.5):

    db = db[collection]
    starttime = date - relativedelta(hours=1,microsecond=0)
    endtime = date
    
    num = int((endtime-starttime).total_seconds()/interval)
    y = [None] * num
  
    #條件
    if ID !=None :
        match = {                                
            'ID':ID,
            'time': {
                '$gte': starttime,
                '$lt': endtime,
            },
            datatype:{'$exists':True}
        }
    else:
        match = {                                
            'time': {
                '$gte': starttime,
                '$lt': endtime,
            }
        }

    #群組
    group =  {
        '_id': ID,
        'time': {  '$push': "$time"  },
        datatype : { '$push': "$"+datatype  }
    }  
    #資料
    data = list(db.aggregate(
        [
            {'$match': match},
            {"$project":{"_id":0,'time':1,datatype:1}},  
            {'$limit' : 86400 *2  },
            {'$group': group},
        ]
    ))
    # print(data[0][datatype])
    if(data!=[]):
        for i,time in enumerate(data[0]['time']):
            diff = time - starttime 
            n = int(diff.total_seconds()/interval) 
            try: 
                y[n] = round(data[0][datatype][i],3)
            except:
                pass
    return y
#---------------------------------------------------------------------------------------zuaibing
def user_set_secondline(db,collection,ID,datatype,date=datetime.datetime.now(),interval=0.5,after_secs=10):

    db = db[collection]
    starttime = date - relativedelta(seconds=after_secs)
    endtime = date
    
    num = int((endtime-starttime).total_seconds()/interval)
    y = [None] * num
  
    #條件
    if ID !=None :
        match = {                                
            'ID':ID,
            'time': {
                '$gte': starttime,
                '$lt': endtime,
            },
            datatype:{'$exists':True}
        }
    else:
        match = {                                
            'time': {
                '$gte': starttime,
                '$lt': endtime,
            }
        }

    #群組
    group =  {
        '_id': ID,
        'time': {  '$push': "$time"  },
        datatype : { '$push': "$"+datatype  }
    }  
    #資料
    data = list(db.aggregate(
        [
            {'$match': match},
            {"$project":{"_id":0,'time':1,datatype:1}},  
            {'$limit' : 86400 *2  },
            {'$group': group},
        ]
    ))
    # print(data[0][datatype])
    if(data!=[]):
        for i,time in enumerate(data[0]['time']):
            diff = time - starttime 
            n = int(diff.total_seconds()/interval) 
            try: 
                y[n] = round(data[0][datatype][i],3)
            except:
                pass
    return y