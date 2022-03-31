import pymongo
import datetime
import time 
from dateutil.relativedelta import relativedelta
from bson.objectid import ObjectId
import urllib3
import json

http = urllib3.PoolManager()
#---------------------------------------------------------------------------------------
def time2string(time,ms=False):
  try:
    if(ms!=True):
      time = datetime.datetime.strftime(time,"%Y-%m-%d %H:%M:%S")
    else:
      time = datetime.datetime.strftime(time,"%Y-%m-%d %H:%M:%S.%f")[:-3]
  except:
    time = ''
  return time
#------------------------------------------------------------------------------------
def beftime(interval = 1):
  localtime = datetime.datetime.now()
  beforetime  = localtime - datetime.timedelta(seconds=interval)    
  return beforetime
#------------------------------------------------------------------------------------
def date_interval(date=datetime.date.today()):
  starttime = datetime.datetime.combine(date, datetime.time.min)
  endtime = starttime+datetime.timedelta(days=1)
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
#--------------------------------------------------------------------------------------
def current_data(db,collection,ID,project={},errortime=None):
  if(errortime!=None):
    beforetime = beftime(errortime)
    match = {'time': {'$gte': beforetime,'$lte': datetime.datetime.now()+relativedelta(minutes=3)}}
  else:
    match = {'time': {'$lte': datetime.datetime.now()+relativedelta(minutes=3)}}
  db = db[collection]
  if(ID==None):
    ID = [None]
  elif(type(ID)==str):
    ID = ID.split(',')
    
  project = {**{'_id':0},**project}
  data = []
  for i in ID:
    if i != None:
      _data = db.find_one({**{'ID':i},**match},project,sort=[( 'time', pymongo.DESCENDING )])
      #_data = db.find_one({'ID':i},project,sort=[( 'time', pymongo.DESCENDING )])
    else:
      _data = db.find_one(match,project,sort=[( 'time', pymongo.DESCENDING )])
      #_data = db.find_one({},project,sort=[( 'time', pymongo.DESCENDING )])
    if(_data!=None):
      data.append(_data)
    else:
      data.append({})
  return data
#----------------------------------------------------------------------------------------
def SOE(db,ID,event,eventType="other"):
  now = datetime.datetime.now()
  db.SOE.insert_one({"ID":ID,"time":now,"event":event,"type":eventType})
  return False
#----------------------------------------------------------------------------------------
def diff_data(db,collection,ID,datatype,date_range=0):
  db = db[collection]
  if(type(ID)==str):
    ID = ID.split(',')
  diff = 0

  if(date_range==3):
    time = {}
  else:
    if(date_range==0):
      starttime,endtime = date_interval()
    if(date_range==1):
      starttime,endtime = month_interval()
    elif(date_range==2):
      starttime,endtime = year_interval()
    time = {'time': {'$gte': starttime,'$lte':endtime}}

  for i in ID:
    for j in db.find({**{'ID':i},**time},{'_id':0,datatype:1}).sort('time',pymongo.DESCENDING).limit(1):
      diff = diff + j[datatype]

    for j in db.find({**{'ID':i},**time},{'_id':0,datatype:1}).sort('time',pymongo.ASCENDING).limit(1):
      diff = diff - j[datatype]
  diff = round(diff,3)
  return diff
#--------------------------------------------------------------------------------------------------------------
def current_state(db,ID):
  db = db['equipment']
  if(type(ID)==str):
    ID = ID.split(',')
  state_data = []
  for i in ID:
    state = db.find_one({'_id':ObjectId(i)},{'_id':0,'state':1})
    if(state=={} or state==None):
      state = {'state':3}
    state_data.append(state)
  return state_data
#--------------------------------------------------------------------------------------------------------------
def current_equip(db,ID):
  db = db['equipment']
  if(type(ID)==str):
    ID = ID.split(',')
  equip_data = []
  for i in ID:
    equip_data.append(db.find_one({'_id':ObjectId(i)},{'_id':0}))
  return equip_data
#--------------------------------------------------------------------------------------------------------------------
def tonum(num):
  try:
    return float(num)
  except:
    return 0
#--------------------------------------------------------------------------------------------------------------------