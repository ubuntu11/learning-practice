#! /usr/bin/env python3
from flask import Flask,jsonify,render_template,request,redirect,url_for,flash,send_from_directory,Response,send_file,session,g,make_response
from flask_login import LoginManager, UserMixin, login_user, login_required, logout_user, current_user
from flask_compress import Compress
import pymongo
from bson.objectid import ObjectId
from config import DevConfig
import json
import time
import datetime
from dateutil.relativedelta import relativedelta
import prepareplot as t
import current as c
import numpy as np
import simplejson

#-------------------------------------------------------------------------------
app = Flask(__name__)
Compress(app)
app.config.from_object(DevConfig)

UPLOAD_FOLDER='upload'
ALLOWED_EXTENSIONS = set(['txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif'])
app.config['UPLOAD_FOLDER']=UPLOAD_FOLDER
ALLOWED_EXTENSIOS= set(['txt'])
#-------------------------------------------------------------------------------
app.secret_key='zxcasdqwe123'
login_manger=LoginManager()
login_manger.session_protection='strong'
login_manger.init_app(app)
#-------------------------------------------------------------------------------
conn = pymongo.MongoClient('mongodb://user:pwd@ems1:27017,ems2:27017', replicaset='rs0',
                           serverSelectionTimeoutMS=10)
#-------------------------------------------------------------------------------
class User(UserMixin):
  pass
#-------------------------------------------------------------------------------
@app.before_request
def before_request():
  session.permanent = True
  app.permanent_session_lifetime = datetime.timedelta(minutes=60)
  session.modified = True
  g.user = current_user 
#-------------------------------------------------------------------------------
#對照是否有此用戶
def user_list(user_id):
  _user=user_id.split('_')
  db =conn[_user[0]]
  for user in db.users.find({'username':user_id.split(_user[0]+'_')[1]}):
    return user
#-------------------------------------------------------------------------------
#如果用戶存在則構建一新用戶類對象，並使用user_id當id
@login_manger.user_loader
def load_user(user_id):
  user=user_list(user_id)
  if user is not None:
    curr_user = User()
    curr_user.id = user['user_id']
    return curr_user
#-------------------------------------------------------------------------------
#當沒登入時導入login頁面
@login_manger.unauthorized_handler
def unauthorized_handler():
  flash('You need login!!')
  return render_template('login.html')
#-------------------------------------------------------------------------------
def check_user():
  user_id = current_user.get_id() 
  # db_username
  try:
    user = user_id.split('_')
  except:
    return None,None
  db = conn[user[0]]
  return [user[0], user_id.split(user[0]+'_')[1]],db
#-------------------------------------------------------------------------------
#404錯誤頁面
#@app.errorhandler(404)
#def page_not_founf(error):
#    return render_template(''),404
#-------------------------------------------------------------------------------
#login
@app.route('/login',methods=['POST'])
def login():
  if request.method == 'POST':
    username = request.form.get('username',type=str)
    company = request.form.get('company',type=str)
    print(username,company)
    if(username!='' and company!=''):
      user_id=company+'_'+username
      user = user_list(user_id)
      #驗證帳號
      if (user!=None) and (request.form['password'] == user['password']):
        print(session)
        curr_user = User()
        curr_user.id = user['user_id']
        #通過flask-login的login_user方法登入用戶
        login_user(curr_user)
        db = conn[company]
        now = datetime.datetime.now()
        c.SOE(db,"system","使用者:"+username+' 登入',"system")
        return redirect('/')
  flash('Wrong infornation!!')
  return render_template('login.html') 
#-------------------------------------------------------------------------------  
#logout
@app.route('/logout')
def logout():
  user,db = check_user()
  if(db!=None):
    now = datetime.datetime.now()
    c.SOE(db,"system","使用者:"+user[1]+' 登出',"system")
  user = current_user
  user.authenticated = False
  logout_user()
  flash('Welcome')
  return render_template('login.html')
#----------------------------------------------------------------------------------
@app.route('/ShowTime')
def ShowTime():
  now = datetime.datetime.now()
  now = datetime.datetime.strftime(now,"%Y-%m-%d %H:%M:%S")
  return jsonify(now = now)
#----------------------------------------------------------------------------------
@app.route('/web_status')
def web_status():
  return 'alive!!'
#############################################################################################################
@app.route('/getSearchListData')
def getSearchListData():
  user,db = check_user()
  if(db == None):
    return logout()
  searchList = json.loads(request.args.get('searchList', type=str))
  data = {}
  equipData = {}
  
  for search in searchList:

    try:
      equip = db.equipment.find_one({"_id":ObjectId(search["ID"])},{"_id":0})
      if(equip!=None):
        equip['ID'] = search["ID"]
        note = db.model.find_one({"ID":equip["model"]},{"_id":0,"tags":1,"error":1})
        if note != None:
          equip.update(note)
        equipData[equip['ID']] = equip
    except:
      print("error")
      pass
    # print(search["col"],search["ID"],search["project"])
    if search["col"]=='SPM':
      data[search["ID"]] = c.current_data(db,search["col"],search["ID"],search["project"],errortime=1350)[0]
      if bool(data[search["ID"]]):
        data[search["ID"]]["SPM"] = round((data[search["ID"]]["SPM"] *100),1)
    else:
      data[search["ID"]] = c.current_data(db,search["col"],search["ID"],search["project"],errortime=60)[0]
    # print(data)
    
  
  data = simplejson.dumps({'data':data, 'equipData':equipData}, ignore_nan=True, default=datetime.datetime.isoformat)
  # return jsonify(data=data,equipData=equipData)
  return data

#--------------------------------------------------------------------------------------------------
@app.route('/history_data_list')
def history_data_list():
  tStart1 = time.time()
  user,db = check_user()
  if(db == None):
    return logout() 
  history_ID = request.args.get('history_ID', type=str)
  datacollection = request.args.get('datacollection', type=str)
  datatype = request.args.get('datatype', type=str)
  print(datatype)
  datepicker1 = request.args.get('datepicker1',type=str)
  datepicker2 = request.args.get('datepicker2',type=str)
  date_range = request.args.get('date_range', type=int,default=0)
  interval = request.args.get('interval', type=int)
  tagType = request.args.get('tagType', type=str)
  plotType = request.args.get('plotType', type=str)
  module_rack_select = request.args.get('module_rack_select', type=int)
  cell_rack_select = request.args.get('cell_rack_select', type=int)
  cell_module_select = request.args.get('cell_module_select', type=int)

  starttime = datetime.datetime.strptime(datepicker1, "%Y-%m-%d %H:%M:%S")
  endtime = datetime.datetime.strptime(datepicker2, "%Y-%m-%d %H:%M:%S") + relativedelta(days=1)
  names = []
  # x_axis = t.fix_x_xis()
  if(datacollection=="mbms"):
    if(tagType == "numberArray"):
      num = db.equipment.find_one({'_id': ObjectId(history_ID)}, {'_id':0, 'rackNum':1})['rackNum']
      name = "Rack"
    elif(tagType == "numberArray2"):
      num = db.equipment.find_one({'_id': ObjectId(history_ID)}, {'_id':0, 'moduleNum':1})['moduleNum']
      name = "Module"
    elif(tagType == "numberArray3"):
      num = db.equipment.find_one({'_id': ObjectId(history_ID)}, {'_id':0, 'cellNum':1})['cellNum']
      name = "Cell"
  if(tagType == "number" and plotType == "scatter"):
    x_axis = t.time_x_xis(starttime,endtime,interval=interval)
    y_axis = t.timeline_one(db,datacollection,history_ID,datatype,starttime,endtime,interval=interval)
  elif(tagType == "numberArray" and plotType == "scatter"):
    x_axis = t.time_x_xis(starttime,endtime,interval=1)
    y_axis, names = t.timeline_one_array(db,datacollection,history_ID,datatype,starttime,endtime,name,num,[],interval=1)
  elif(tagType == "numberArray2" and plotType == "scatter"):
    x_axis = t.time_x_xis(starttime,endtime,interval=1)
    y_axis, names = t.timeline_one_array(db,datacollection,history_ID,datatype,starttime,endtime,name,num,[module_rack_select],interval=1)
  elif(tagType == "numberArray3" and plotType == "scatter"):
    x_axis = t.time_x_xis(starttime,endtime,interval=1)
    y_axis, names = t.timeline_one_array(db,datacollection,history_ID,datatype,starttime,endtime,name,num,[cell_rack_select,cell_module_select],interval=1)
  elif(tagType == "number" and plotType == "bar"):
    x_axis = t.bar_x_xis(starttime,endtime,date_range)
    y_axis = t.bar_one(db,datacollection,history_ID,datatype,starttime,endtime,date_range)
  else:
    x_axis = t.time_x_xis(starttime,endtime,interval=1)
    y_axis = [100]*len(x_axis)
  tEnd1 = time.time()#計時結束
  print(np.array(y_axis).shape)
  # print(y_axis)
  #列印結果
  print("status cost %f sec" % (tEnd1 - tStart1),"SSSSSSSSSSSSSSSSSSSSSs")#會自動做近位  
  return jsonify(x_axis=x_axis,y_axis=y_axis, name=names)
#--------------------------------------------------------------------------------------------------
@app.route('/sys_data_list')
def sys_data_list():
  tStart1 = time.time()
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = "iec61850"
  datacollection = "iec61850"
  datatype = ["second.7", "executive_rate", "second.6"]
  starttime = request.args.get('starttime',type=str)
  endtime = request.args.get('endtime',type=str)
  interval = 1
  tagType = "number"
  plotType = 'scatter'
  # x_axis = t.fix_x_xis()
  if endtime is None:
    endtime = datetime.datetime.now().replace(microsecond=0) + relativedelta(seconds=-2)
  else:
    endtime = datetime.datetime.strptime(endtime, "%Y-%m-%d %H:%M:%S") + relativedelta(days=1)
  if starttime is None:
    starttime = endtime - relativedelta(hours=3)
  else:
    starttime = datetime.datetime.strptime(starttime, "%Y-%m-%d %H:%M:%S")
    if endtime is None:
      starttime +=  relativedelta(seconds=interval)
  print(starttime, endtime)
  # endtime += relativedelta(seconds=1)
  if(tagType == "number" and plotType == "scatter"):
    x_axis = t.time_x_xis(starttime,endtime)
    y_axis = t.timeline_multi(db,datacollection,ID,datatype,starttime,endtime,interval=1)
    # print(x_axis, y_axis)
    # print("sssssssssssssssssssssssssssssssssssssssssssdddddddddddddddddddddddddd",y_axis)
    site_control = db["site_control"].find_one({}, sort=[("time", -1)])
    # if site_control["mode"] == 5 or site_control["mode"] == 11:
    #   # y_length = len(y_axis[0])
    #   y_axis.append([site_control["f6_line_set"]])
    #   y_axis.append([site_control["f5_line_set"]])
    #   y_axis.append([site_control["f4_line_set"]])
    #   y_axis.append([site_control["f3_line_set"]])
    #   y_axis.append([site_control["f2_line_set"]])
    #   y_axis.append([site_control["f1_line_set"]])
  tEnd1 = time.time()#計時結束
  #列印結果
  print(len(x_axis))
  print("status cost %f sec" % (tEnd1 - tStart1),"SSSSSSSSSSSSSSSSSSSSSs")#會自動做近位  
  return jsonify(x_axis=x_axis,y_axis=y_axis)
#--------------------------------------------------------------------------------------------------
@app.route('/alarm_get')
def alarm_get():
  user,db = check_user()
  if(db == None):
    return logout() 
  limit = request.args.get('limit',type=int)
  start = request.args.get('start',type=int)
  page = request.args.get('page',type=int)
  condition = json.loads(request.args.get('condition', type=str))
  datepicker1 = request.args.get('datepicker1', type=str)
  datepicker2 = request.args.get('datepicker2', type=str)
  search = request.args.get('search', type=int)
  skip = page * limit
  # print(limit,start,page)
  data=[]
  total = 0
  if((condition['show']["1"]==False and condition['show']["2"]==False) or (condition['level']["1"]==False and condition['level']["2"]==False)):
    pass
  else:
    show = []
    level = []
    if(condition['show']["1"]==True):
      show.append(1)
    if(condition['show']["2"]==True):
      show.append(2)
    if(condition['level']["1"]==True):
      level.append(1)
    if(condition['level']["2"]==True):
      level.append(2)

    time_condiction = {}
    if(search==1):
      starttime = datetime.datetime.strptime(datepicker1+' 00:00:00', "%Y-%m-%d %H:%M:%S")
      endtime = datetime.datetime.strptime(datepicker2+' 00:00:00', "%Y-%m-%d %H:%M:%S")
      endtime = endtime + datetime.timedelta(days=1)
      time_condiction = {'time': {'$gte': starttime,'$lte':endtime}}
    elif(search==2):
      starttime = datetime.datetime.combine(datetime.date.today(), datetime.time.min)
      endtime = starttime + datetime.timedelta(days=1)
      time_condiction = {'time': {'$gte': starttime,'$lte':endtime}}
    checktime_condiction = {}
    if(condition.get('checktime',"") == None):
      checktime_condiction = { "checktime":None}
    condiction_list = []
    equip_list = list(db.equipment.find({}))
    equip_list += list(db.program.find({}))
    equip_list += [{"_id":None,"ID":"system", "name":"系統", "place":"系統", "show":1, "level":1}]
    equipment_dict = {}
   
    for i in equip_list:
      if 'ID' not in i:
        i['ID'] = str(i['_id'])
      i.pop('_id')
      equipment_dict[i['ID']] = i  
      for j in show:
        for k in level:
          condiction_list.append({'ID':i['ID'],'show':j,'level':k})
    if(condiction_list !=[]):
      print({**time_condiction,**checktime_condiction})
      total = db.alarm.find({**time_condiction,**checktime_condiction,'$or':condiction_list},{'_id':1}).count()
      for i in db.alarm.aggregate([
        {'$match':{**time_condiction,**checktime_condiction,'$or':condiction_list}},
        {'$sort':{'show':1,"time":-1,"_id":-1}},
        {'$skip':skip},
        {'$limit':limit},
        {'$project': {
          '_id': {'$toString': "$_id"},
          'ID':1,
          'event':1,
          'value':1,
          'level':1,
          'levelText':{
            '$switch': {
              'branches': [
                { 'case': { '$eq': [  "$level", 1 ] }, 'then': "警告" },
                { 'case': { '$eq': [  "$level", 2 ] }, 'then': "故障" },
              ],
              'default': "Fault"
            }
          },
          "show":1,
          'time': { '$dateToString': { 'date': "$time",'format': "%Y-%m-%d %H:%M:%S" ,'onNull': ''} },
          'checktime': { '$dateToString': {  'date': "$checktime",'format': "%Y-%m-%d %H:%M:%S",'onNull': ''} },
          'returntime': { '$dateToString': { 'date': "$returntime",'format': "%Y-%m-%d %H:%M:%S",'onNull': ''} },
          'place':"equipment_dict[event['ID']]['place']",
          # "equip":{ '$cond': [  [{ '$type': "$ID" }, "string" ], "equipment_dict['$ID']['name']", "" ] }
        }},
      ]):
        start = start+1
        i['index'] = start
        i['equip'] = equipment_dict[i['ID']]['name']
        i['place'] = equipment_dict[i['ID']]['place']
        data.append(i) 
  return jsonify(data=data, total=total)
#--------------------------------------------------------------------------------------------------
@app.route('/protection_get')
def protection_get():
  user,db = check_user()  
  if(db == None):
    return logout()
  data = c.current_data(db,"sys_control","system")[0]
  return jsonify(data=data)
#--------------------------------------------------------------------------------------------------
@app.route('/equipment_status_list')
def equipment_status_list():
  user,db = check_user()  
  if(db == None):
    return logout()
  status_ID = json.loads(request.args.get('status_ID', type=str))
  statuss = db.equipment_status.find({'ID':{'$in': status_ID}}, {'status':1, 'ID':1, '_id':0})
  # print(list(statuss))
  ID_statuss = {}
  for status in statuss:
    if status['status'][1]>0:
      ID_statuss[status['ID']] = status['status'][1]+1
    else:
      ID_statuss[status['ID']] = status['status'][0]
  # statuss = list(status['status'][1]+1 if status['status'][1]>0 else status['status'][0] for status in statuss)
  # print(ID_statuss)
  return jsonify(statuss=ID_statuss)
#--------------------------------------------------------------------------------------------------
@app.route('/pcs_status_list')
def pcs_status_list():
  user,db = check_user()  
  if(db == None):
    return logout()
  status_ID = json.loads(request.args.get('status_ID', type=str))
  # status_ID = ["em1","ems2","gc1","gc2","dc","mbms","hmi","gc3"]
  # statuss = db.equipment_status.find({'ID':{'$in': status_ID}}, {'status':1, 'ID':1, '_id':0})
  try:
    statuss = c.current_data(db,'pc_info',status_ID,errortime=300)
  except:
    statuss = [{}]*len(status_ID)
  ID_statuss = {}
  for idx, status in enumerate(statuss):
    if bool(status):
      ID_statuss[status_ID[idx]] = 0
    else:
      ID_statuss[status_ID[idx]] = 1
  # statuss = list(status['status'][1]+1 if status['status'][1]>0 else status['status'][0] for status in statuss)
  print(ID_statuss)
  return jsonify(statuss=ID_statuss)
#############################################################################################################
@app.route('/')
@login_required
def index():
  user,db = check_user()
  if(db == None):
    return logout() 
  for user in db.users.find({'username':user[1]}):
    username = user['username']
    level = user['level']
  return render_template('index.html',username=username,level=level)
#--------------------------------------------------------------------------------------------------
@app.route('/info')
@login_required
def info():
  user,db = check_user()
  if(db == None):
    return logout() 
  equipType = request.args.get('equipType',type=str)
  for user in db.users.find({'username':user[1]}):
    username = user['username']
    level = user['level']
  return render_template('info.html',username=username,level=level,equipType=equipType)

#--------------------------------------------------------------------------------------------------
if __name__ == '__main__':
  app.run(host='0.0.0.0',port='8080')
  # app.run(host='127.0.0.1',port='8080') #test only#