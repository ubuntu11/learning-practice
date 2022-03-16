#! /usr/bin/env python3
from flask import Flask,jsonify,render_template,request,redirect,url_for,flash,send_from_directory,Response,send_file,session,g,make_response
from flask_login import LoginManager, UserMixin, login_user, login_required, logout_user, current_user
from flask_compress import Compress
import pymongo
import re #like find
from bson.objectid import ObjectId
import os
from config import DevConfig
import json
import time
import datetime
from dateutil.relativedelta import relativedelta
import math
import csv
import io
import prepareplot as t
import current as c
import pandas as pd 
import numpy as np
# from pm_import_test import caculate_pm, avg_pm
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
# conn = pymongo.MongoClient('mongodb://user:pwd@127.0.0.1:27017, 127.0.0.1:27018', replicaset='rs0', serverSelectionTimeoutMS=10) 
conn = pymongo.MongoClient('mongodb://user:pwd@127.0.0.1:27017', serverSelectionTimeoutMS=10) 
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
#-------------------------------------------------------------------------------------------------
@app.route('/get_info_equipList')
def get_info_equipList():
  user,db = check_user()
  if(db == None):
    return logout()
  data = []
  equipType = request.args.get('equipType', type=str)
  equipTypeList = []
  if(equipType == "mbms"):
    equipTypeList = ["mbms"]
  elif(equipType == "pcs"):
    equipTypeList = ["pcs"]
  elif(equipType == "acm"):
    equipTypeList = ["acm","protective_relay"]
  elif(equipType == "containter"):
    equipTypeList = ["ups","air","env","rio"]
  for i in equipTypeList:
    for j in db.equipment.find({"type":i}).sort("num",1):
      if j['rw'] == 1 or j['rw'] == 3:
        j['ID'] = str(j['_id'])
        j.pop('_id')
        note = db.model.find_one({"ID":j["model"]},{"_id":0,"tags":1,"error":1})
        if note != None:
          j.update(note)
        data.append(j)
  return jsonify(data=data,equipType=equipType)
#--------------------------------------------------------------------------------------------------
@app.route('/get_control_equipList')
def get_control_equipList():
  user,db = check_user()
  if(db == None):
    return logout()
  data = []
  equipType = request.args.get('equipType', type=str)
  equipTypeList = []
  if(equipType == "site_control"):
    # {"ID":"site_control","name":"系統模式","type":"site_control"}
    equipTypeList = ["pcs"]
  elif(equipType == "mbms"):
    equipTypeList = ["mbms"]
  elif(equipType == "pcs"):
    equipTypeList = ["pcs"]
  elif(equipType == "containter"):
    equipTypeList = ["air","rio"]

  for i in equipTypeList:
    for j in db.equipment.find({"type":i}).sort("num",1):
      if j['rw'] == 2 or j['rw'] == 3:
        j['ID'] = str(j['_id'])
        j.pop('_id')
        note = db.model.find_one({"ID":j["model"]},{"_id":0,"tags":1,"error":1})
        if note != None:
          j.update(note)
        data.append(j)
  return jsonify(data=data,equipType=equipType)
#--------------------------------------------------------------------------------------------------
@app.route('/get_mbms_data')
def get_mbms_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  # data = c.current_data(db,"mbms",ID,errortime=60)[0]
  data = c.current_data(db,"mbms",ID,errortime=60)[0] #test only#
  return jsonify(data = data)
#--------------------------------------------------------------------------------------------------
@app.route('/get_pcs_data')
def get_pcs_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"pcs",ID,errortime=60)[0]
  return jsonify(data = data)
#--------------------------------------------------------------------------------------------------
@app.route('/get_ups_data')
def get_ups_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"ups",ID,errortime=60)[0]
  return jsonify(data = data)
#--------------------------------------------------------------------------------------------------
@app.route('/get_env_data')
def get_env_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"env",ID,errortime=60)[0]
  return jsonify(data = data)
#--------------------------------------------------------------------------------------------------
@app.route('/get_protective_relay_data')
def get_protective_relay_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"protective_relay",ID,errortime=60)[0]
  return jsonify(data = data)
#--------------------------------------------------------------------------------------------------
@app.route('/get_rio_data')
def get_rio_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"rio",ID,errortime=60)[0]
  return jsonify(data = data)
  #--------------------------------------------------------------------------------------------------
@app.route('/get_air_data')
def get_air_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"air",ID,errortime=60)[0]
  return jsonify(data = data)
#--------------------------------------------------------------------------------------------------
@app.route('/get_acm_data')
def get_acm_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = {"data":c.current_data(db,"acm",ID,errortime=60)[0]}
  data = simplejson.dumps(data, ignore_nan=True, default=datetime.datetime.isoformat)
  # print(data)
  return data
#--------------------------------------------------------------------------------------------------
@app.route('/get_mbms_control_data')
def get_mbms_control_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"mbms_control",ID)[0]
  return jsonify(data = data)
#--------------------------------------------------------------------------------------------------
@app.route('/get_pcs_control_data')
def get_pcs_control_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"pcs_control",ID)[0]
  return jsonify(data = data)
#--------------------------------------------------------------------------------------------------
@app.route('/get_air_control_data')
def get_air_control_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  ID = request.args.get('ID',type=str)
  data = c.current_data(db,"air_control",ID)[0]
  return jsonify(data = data)
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
#取得site_control mode資料
@app.route('/site_control_mode_data')
def site_control_mode_data():
  user,db = check_user()
  if(db == None):
    return logout() 
  mode_data = c.current_data(db,'site_control','site_control')[0]
  return jsonify(mode_data=mode_data)
#-------------------------------------------------------------------------------------------------------------------
#設定site_control 模式參數
@app.route('/site_control_set_mode')
def site_control_set_mode():
  user,db = check_user()
  if(db == None):
    return logout() 
  mode_data = json.loads(request.args.get('mode_data', type=str))
  print(mode_data)
  mode_data_old = c.current_data(db,'site_control',"site_control")[0]
  # 移除GC識別用資料
  try:
    mode_data_old.pop("response")
  except:
    pass
  try:
    mode_data_old.pop("local")
  except:
    pass
  #
  now = datetime.datetime.now()
  sys_set = c.current_data(db,'sys_control','system')[0]
  protection = sys_set.get("protection",1)
  status = "失敗"
  mode_data_old['ID'] = "site_control"
  if(protection==1 or protection==2):
    print("!")
    c.SOE(db,"site_control","設定系統模式參數失敗,原因為系統保護中","systemMode")
    status = "系統保護中"
  else:
    # print("!!")
    if(mode_data['mode']=='0'):
      mode_data_old['mode'] = 0
      mode_data_old['scheduleFlag'] = 0
      mode_data_old['time'] = now
      event = "設定系統模式參數(模式:停止模式)"
    else:
      # try:
      #     if(mode_data_old["bess_p_max"] != c.tonum(mode_data["bess_p_max"])):
      #         event = "設定系統模式參數限制值(系統最大輸出功率:"+mode_data['bess_p_max']+"kW)"
      #         c.SOE(db,"site_control",event,"systemMode")
      # except:
      #     pass
      try:
        if(mode_data_old["soc_max"] != c.tonum(mode_data["soc_max"])):
          event = "設定系統模式參數限制值(最高SOC:"+mode_data['soc_max']+"%)"
          c.SOE(db,"site_control",event,"systemMode")
      except:
        pass
      try:
        if(mode_data_old["soc_min"] != c.tonum(mode_data["soc_min"])):
          event = "設定系統模式參數限制值(最低SOC:"+mode_data['soc_min']+"%)"
          c.SOE(db,"site_control",event,"systemMode")
      except:
        pass
      try:
        if(mode_data_old["System_p_max"] != c.tonum(mode_data["System_p_max"])):
          event = "設定系統模式參數限制值(最高放電實功:"+mode_data['System_p_max']+"kW)"
          c.SOE(db,"site_control",event,"systemMode")
      except:
        pass
      try:
        if(mode_data_old["System_p_min"] != c.tonum(mode_data["System_p_min"])):
          event = "設定系統模式參數限制值(最高充電實功:"+mode_data['System_p_min']+"kW)"
          c.SOE(db,"site_control",event,"systemMode")
      except:
          pass
      try:
        if(mode_data_old["System_q_max"] != c.tonum(mode_data["System_q_max"])):
          event = "設定系統模式參數限制值(最高虛功:"+mode_data['System_q_max']+"kVAR)"
          c.SOE(db,"site_control",event,"systemMode")
      except:
          pass
      try:
        if(mode_data_old["System_q_min"] != c.tonum(mode_data["System_q_min"])):
          event = "設定系統模式參數限制值(最低虛功:"+mode_data['System_q_min']+"kVAR)"
          c.SOE(db,"site_control",event,"systemMode")
      except:
        pass

      # mode_data_old['bess_p_max'] = c.tonum(mode_data['bess_p_max'])
      mode_data_old['soc_max'] = c.tonum(mode_data['soc_max'])
      mode_data_old['soc_min'] =  c.tonum(mode_data['soc_min'])
      mode_data_old['System_p_max'] = c.tonum(mode_data['System_p_max'])
      mode_data_old['System_p_min'] = c.tonum(mode_data['System_p_min'])
      mode_data_old['System_q_max'] = c.tonum(mode_data['System_q_max'])
      mode_data_old['System_q_min'] = c.tonum(mode_data['System_q_min'])
      mode_data_old['time'] = now
      if(mode_data['mode']=='1'):
        mode_data_old['mode'] = 1
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['PF_p_ref'] = c.tonum(mode_data['PF_p_ref'])
        mode_data_old['PF_pf_ref'] = c.tonum(mode_data['PF_pf_ref'])
        event = "設定系統模式參數(模式:固定功因模式, 實功:"+mode_data['PF_p_ref']+"kW, 功因:"+mode_data['PF_pf_ref']+")"
      elif(mode_data['mode']=='2'):
        mode_data_old['mode'] = 2
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['Vq_v1_set'] = c.tonum(mode_data['Vq_v1_set'])
        mode_data_old['Vq_v2_set'] = c.tonum(mode_data['Vq_v2_set'])
        mode_data_old['Vq_v3_set'] = c.tonum(mode_data['Vq_v3_set'])
        mode_data_old['Vq_v4_set'] = c.tonum(mode_data['Vq_v4_set'])
        mode_data_old['Vq_v5_set'] = c.tonum(mode_data['Vq_v5_set'])
        mode_data_old['Vq_v6_set'] = c.tonum(mode_data['Vq_v6_set'])
        mode_data_old['Vq_v7_set'] = c.tonum(mode_data['Vq_v7_set'])
        mode_data_old['Vq_q1_set'] = c.tonum(mode_data['Vq_q1_set'])
        mode_data_old['Vq_q2_set'] = c.tonum(mode_data['Vq_q2_set'])
        mode_data_old['Vq_q3_set'] = c.tonum(mode_data['Vq_q3_set'])
        mode_data_old['Vq_q4_set'] = c.tonum(mode_data['Vq_q4_set'])
        mode_data_old['Vq_q5_set'] = c.tonum(mode_data['Vq_q5_set'])
        mode_data_old['Vq_q6_set'] = c.tonum(mode_data['Vq_q6_set'])
        mode_data_old['Vq_q7_set'] = c.tonum(mode_data['Vq_q7_set'])
        mode_data_old['Vq_q_base'] = c.tonum(mode_data['Vq_q_base'])
        mode_data_old['Vq_v_base'] = c.tonum(mode_data['Vq_v_base'])
        event = "設定系統模式參數(模式:電壓虛功模式, 電壓參考值:"+mode_data['Vq_v_base']+"kV, 虛功參考值:"+mode_data['Vq_q_base']+"kVAR"
        event += ", VQ1:("+mode_data['Vq_v1_set']+","+mode_data['Vq_q1_set']+")"
        event += ", VQ2:("+mode_data['Vq_v2_set']+","+mode_data['Vq_q2_set']+")"
        event += ", VQ3:("+mode_data['Vq_v3_set']+","+mode_data['Vq_q3_set']+")"
        event += ", VQ4:("+mode_data['Vq_v4_set']+","+mode_data['Vq_q4_set']+")"
        event += ", VQ5:("+mode_data['Vq_v5_set']+","+mode_data['Vq_q5_set']+")"
        event += ", VQ6:("+mode_data['Vq_v6_set']+","+mode_data['Vq_q6_set']+")"
        event += ", VQ7:("+mode_data['Vq_v7_set']+","+mode_data['Vq_q7_set']+")"
        event += " )"
      elif(mode_data['mode']=='3'):
        mode_data_old['mode'] = 3
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['Vp_v1_set'] = c.tonum(mode_data['Vp_v1_set'])
        mode_data_old['Vp_v2_set'] = c.tonum(mode_data['Vp_v2_set'])
        mode_data_old['Vp_v3_set'] = c.tonum(mode_data['Vp_v3_set'])
        mode_data_old['Vp_v4_set'] = c.tonum(mode_data['Vp_v4_set'])
        mode_data_old['Vp_v5_set'] = c.tonum(mode_data['Vp_v5_set'])
        mode_data_old['Vp_v6_set'] = c.tonum(mode_data['Vp_v6_set'])
        mode_data_old['Vp_p1_set'] = c.tonum(mode_data['Vp_p1_set'])
        mode_data_old['Vp_p2_set'] = c.tonum(mode_data['Vp_p2_set'])
        mode_data_old['Vp_p3_set'] = c.tonum(mode_data['Vp_p3_set'])
        mode_data_old['Vp_p4_set'] = c.tonum(mode_data['Vp_p4_set'])
        mode_data_old['Vp_p5_set'] = c.tonum(mode_data['Vp_p5_set'])
        mode_data_old['Vp_p6_set'] = c.tonum(mode_data['Vp_p6_set'])
        mode_data_old['Vp_p_base'] = c.tonum(mode_data['Vp_p_base'])
        mode_data_old['Vp_v_base'] = c.tonum(mode_data['Vp_v_base'])
        event = "設定系統模式參數(模式:電壓實功模式, 電壓參考值:"+mode_data['Vp_v_base']+"kV, 實功參考值:"+mode_data['Vp_p_base']+"kW"
        event += ", VP1:("+mode_data['Vp_v1_set']+","+mode_data['Vp_p1_set']+")"
        event += ", VP2:("+mode_data['Vp_v2_set']+","+mode_data['Vp_p2_set']+")"
        event += ", VP3:("+mode_data['Vp_v3_set']+","+mode_data['Vp_p3_set']+")"
        event += ", VP4:("+mode_data['Vp_v4_set']+","+mode_data['Vp_p4_set']+")"
        event += ", VP5:("+mode_data['Vp_v5_set']+","+mode_data['Vp_p5_set']+")"
        event += ", VP6:("+mode_data['Vp_v6_set']+","+mode_data['Vp_p6_set']+")"
        event += " )"
      elif(mode_data['mode']=='4'):
        mode_data_old['mode'] = 4
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['Vpq_p_v1_set'] = c.tonum(mode_data['Vpq_p_v1_set'])
        mode_data_old['Vpq_p_v2_set'] = c.tonum(mode_data['Vpq_p_v2_set'])
        mode_data_old['Vpq_p_v3_set'] = c.tonum(mode_data['Vpq_p_v3_set'])
        mode_data_old['Vpq_p_v4_set'] = c.tonum(mode_data['Vpq_p_v4_set'])
        mode_data_old['Vpq_p_p1_set'] = c.tonum(mode_data['Vpq_p_p1_set'])
        mode_data_old['Vpq_p_p2_set'] = c.tonum(mode_data['Vpq_p_p2_set'])
        mode_data_old['Vpq_p_p3_set'] = c.tonum(mode_data['Vpq_p_p3_set'])
        mode_data_old['Vpq_p_p4_set'] = c.tonum(mode_data['Vpq_p_p4_set'])
        mode_data_old['Vpq_q_v1_set'] = c.tonum(mode_data['Vpq_q_v1_set'])
        mode_data_old['Vpq_q_v2_set'] = c.tonum(mode_data['Vpq_q_v2_set'])
        mode_data_old['Vpq_q_v3_set'] = c.tonum(mode_data['Vpq_q_v3_set'])
        mode_data_old['Vpq_q_v4_set'] = c.tonum(mode_data['Vpq_q_v4_set'])
        mode_data_old['Vpq_q_q1_set'] = c.tonum(mode_data['Vpq_q_q1_set'])
        mode_data_old['Vpq_q_q2_set'] = c.tonum(mode_data['Vpq_q_q2_set'])
        mode_data_old['Vpq_q_q3_set'] = c.tonum(mode_data['Vpq_q_q3_set'])
        mode_data_old['Vpq_q_q4_set'] = c.tonum(mode_data['Vpq_q_q4_set'])
        mode_data_old['Vpq_q_base'] = c.tonum(mode_data['Vpq_q_base'])
        mode_data_old['Vpq_p_base'] = c.tonum(mode_data['Vpq_p_base'])
        mode_data_old['Vpq_v_base'] = c.tonum(mode_data['Vpq_v_base'])
        event = "設定系統模式參數(模式:電壓實虛功模式, 電壓參考值:"+mode_data['Vp_v_base']+"kV, 實功參考值:"+mode_data['Vp_p_base']+"kW, 虛功參考值:"+mode_data['Vq_q_base']+"kVAR"
        event += ", VP1:("+mode_data['Vpq_p_v1_set']+","+mode_data['Vpq_p_p1_set']+")"
        event += ", VP2:("+mode_data['Vpq_p_v2_set']+","+mode_data['Vpq_p_p2_set']+")"
        event += ", VP3:("+mode_data['Vpq_p_v3_set']+","+mode_data['Vpq_p_p3_set']+")"
        event += ", VP4:("+mode_data['Vpq_p_v1_set']+","+mode_data['Vpq_p_p4_set']+")"
        event += ", VQ1:("+mode_data['Vpq_q_v1_set']+","+mode_data['Vpq_q_q1_set']+")"
        event += ", VQ2:("+mode_data['Vpq_q_v2_set']+","+mode_data['Vpq_q_q2_set']+")"
        event += ", VQ3:("+mode_data['Vpq_q_v3_set']+","+mode_data['Vpq_q_q3_set']+")"
        event += ", VQ4:("+mode_data['Vpq_q_v4_set']+","+mode_data['Vpq_q_q4_set']+")"
        event += " )"
      elif(mode_data['mode']=='5'):
        mode_data_old['mode'] = 5
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['f1_line_set'] = c.tonum(mode_data['f1_line_set'])
        mode_data_old['f2_line_set'] = c.tonum(mode_data['f2_line_set'])
        mode_data_old['f3_line_set'] = c.tonum(mode_data['f3_line_set'])
        mode_data_old['f4_line_set'] = c.tonum(mode_data['f4_line_set'])
        mode_data_old['f5_line_set'] = c.tonum(mode_data['f5_line_set'])
        mode_data_old['f6_line_set'] = c.tonum(mode_data['f6_line_set'])
        mode_data_old['p1_line_set'] = c.tonum(mode_data['p1_line_set'])
        mode_data_old['p2_line_set'] = c.tonum(mode_data['p2_line_set'])
        mode_data_old['p3_line_set'] = c.tonum(mode_data['p3_line_set'])
        mode_data_old['p4_line_set'] = c.tonum(mode_data['p4_line_set'])
        mode_data_old['p5_line_set'] = c.tonum(mode_data['p5_line_set'])
        mode_data_old['p6_line_set'] = c.tonum(mode_data['p6_line_set'])
        mode_data_old['p6_line_set'] = c.tonum(mode_data['p6_line_set'])
        mode_data_old['FP_line_p_base'] = c.tonum(mode_data['FP_line_p_base'])
        mode_data_old['FP_type'] = c.tonum(mode_data['FP_type'])
        mode_data_old['FP_soc_goal_percent'] = c.tonum(mode_data['FP_soc_goal_percent'])
        mode_data_old['FP_main_f_set'] = c.tonum(mode_data['FP_main_f_set'])
        mode_data_old['FP_main_f_set_source'] = c.tonum(mode_data['FP_main_f_set_source'])
        event = "設定系統模式參數(模式:頻率實功模式, 功率參考值:"+mode_data['FP_line_p_base']+"kW"
        event += ", SoC目標值:"+mode_data['FP_soc_goal_percent']+"%"
        event += ", 頻率基準:"+mode_data['FP_main_f_set']+"Hz"
        event += ", 啟用台電移頻指令:"+ str(mode_data['FP_main_f_set_source'])
        event += ", FP1:("+mode_data['f1_line_set']+","+mode_data['p1_line_set']+")"
        event += ", FP2:("+mode_data['f2_line_set']+","+mode_data['p2_line_set']+")"
        event += ", FP3:("+mode_data['f3_line_set']+","+mode_data['p3_line_set']+")"
        event += ", FP4:("+mode_data['f4_line_set']+","+mode_data['p4_line_set']+")"
        event += ", FP5:("+mode_data['f5_line_set']+","+mode_data['p5_line_set']+")"
        event += ", FP6:("+mode_data['f6_line_set']+","+mode_data['p6_line_set']+")"
        event += " )"
      elif(mode_data['mode']=='6'):
        mode_data_old['mode'] = 6
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['PQ_p_ref'] = c.tonum(mode_data['PQ_p_ref'])
        mode_data_old['PQ_q_ref'] = c.tonum(mode_data['PQ_q_ref'])
        event = "設定系統模式參數(模式:實虛功控制模式, 實功:"+mode_data['PQ_p_ref']+"kW, 虛功:"+mode_data['PQ_q_ref']+"kVAR)"
      elif(mode_data['mode']=='7'):
        mode_data_old['mode'] = 7
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['Stable_p_tr_new'] = c.tonum(mode_data['Stable_p_tr_new'])
        mode_data_old['Stable_p_ramp'] = c.tonum(mode_data['Stable_p_ramp'])
        mode_data_old['PV_Steady_Capacity'] = c.tonum(mode_data['PV_Steady_Capacity'])
        event = "設定系統模式參數(模式:穩定實功模式, PV_Steady_Capacity:"+mode_data['PV_Steady_Capacity']+"kW, 實功:"+mode_data['Stable_p_tr_new']+"kW, 升降載率:"+mode_data['Stable_p_ramp']+"%)"
      elif(mode_data['mode']=='8'):
        mode_data_old['mode'] = 8
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['Smooth_p_variance'] = c.tonum(mode_data['Smooth_p_variance'])
        mode_data_old['PV_Smooth_Capacity'] = c.tonum(mode_data['PV_Smooth_Capacity'])
        event = "設定系統模式參數(模式:平滑化模式, PV_Smooth_Capacity:"+mode_data['PV_Smooth_Capacity']+"kW, 功率變動率:"+mode_data['Smooth_p_variance']+"%)"
      elif(mode_data['mode']=='9'):
        mode_data_old['mode'] = 9
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['Anti_p_limit_new'] = c.tonum(mode_data['Anti_p_limit_new'])    
        mode_data_old['Power_Reverse_Watt_Limit'] = c.tonum(mode_data['Power_Reverse_Watt_Limit'])    
        mode_data_old['PV_Reverse_Capacity'] = c.tonum(mode_data['PV_Reverse_Capacity'])
        event = "設定系統模式參數(模式:功率逆送模式, PV_Reverse_Capacity:"+mode_data['PV_Reverse_Capacity']+"kW, 逆送實功限制:"+mode_data['Anti_p_limit_new']+"kW, 降載率:"+mode_data['Power_Reverse_Watt_Limit']+"%)"
      elif(mode_data['mode']=='10'):
        mode_data_old['mode'] = 10
        mode_data_old['scheduleFlag'] = 0
        event = "設定系統模式參數(模式:SOC校正模式)"
      elif(mode_data['mode']=='11'):
        mode_data_old['mode'] = 11
        mode_data_old['scheduleFlag'] = 0
        mode_data_old['Vq_v1_set'] = c.tonum(mode_data['Vq_v1_set'])
        mode_data_old['Vq_v2_set'] = c.tonum(mode_data['Vq_v2_set'])
        mode_data_old['Vq_v3_set'] = c.tonum(mode_data['Vq_v3_set'])
        mode_data_old['Vq_v4_set'] = c.tonum(mode_data['Vq_v4_set'])
        mode_data_old['Vq_v5_set'] = c.tonum(mode_data['Vq_v5_set'])
        mode_data_old['Vq_v6_set'] = c.tonum(mode_data['Vq_v6_set'])
        mode_data_old['Vq_v7_set'] = c.tonum(mode_data['Vq_v7_set'])
        mode_data_old['Vq_q1_set'] = c.tonum(mode_data['Vq_q1_set'])
        mode_data_old['Vq_q2_set'] = c.tonum(mode_data['Vq_q2_set'])
        mode_data_old['Vq_q3_set'] = c.tonum(mode_data['Vq_q3_set'])
        mode_data_old['Vq_q4_set'] = c.tonum(mode_data['Vq_q4_set'])
        mode_data_old['Vq_q5_set'] = c.tonum(mode_data['Vq_q5_set'])
        mode_data_old['Vq_q6_set'] = c.tonum(mode_data['Vq_q6_set'])
        mode_data_old['Vq_q7_set'] = c.tonum(mode_data['Vq_q7_set'])
        mode_data_old['Vq_q_base'] = c.tonum(mode_data['Vq_q_base'])
        mode_data_old['Vq_v_base'] = c.tonum(mode_data['Vq_v_base'])
        mode_data_old['f1_line_set'] = c.tonum(mode_data['f1_line_set'])
        mode_data_old['f2_line_set'] = c.tonum(mode_data['f2_line_set'])
        mode_data_old['f3_line_set'] = c.tonum(mode_data['f3_line_set'])
        mode_data_old['f4_line_set'] = c.tonum(mode_data['f4_line_set'])
        mode_data_old['f5_line_set'] = c.tonum(mode_data['f5_line_set'])
        mode_data_old['f6_line_set'] = c.tonum(mode_data['f6_line_set'])
        mode_data_old['p1_line_set'] = c.tonum(mode_data['p1_line_set'])
        mode_data_old['p2_line_set'] = c.tonum(mode_data['p2_line_set'])
        mode_data_old['p3_line_set'] = c.tonum(mode_data['p3_line_set'])
        mode_data_old['p4_line_set'] = c.tonum(mode_data['p4_line_set'])
        mode_data_old['p5_line_set'] = c.tonum(mode_data['p5_line_set'])
        mode_data_old['p6_line_set'] = c.tonum(mode_data['p6_line_set'])
        mode_data_old['FP_line_p_base'] = c.tonum(mode_data['FP_line_p_base'])
        mode_data_old['FP_type'] = c.tonum(mode_data['FP_type'])
        mode_data_old['FP_soc_goal_percent'] = c.tonum(mode_data['FP_soc_goal_percent'])
        mode_data_old['FP_main_f_set'] = c.tonum(mode_data['FP_main_f_set'])
        mode_data_old['FP_main_f_set_source'] = c.tonum(mode_data['FP_main_f_set_source'])
        event = "設定系統模式參數(模式:AFC模式, 功率參考值:"+mode_data['FP_line_p_base']+"kW"
        event += ", SoC目標值:"+mode_data['FP_soc_goal_percent']+"%"
        event += ", 頻率基準:"+mode_data['FP_main_f_set']+"Hz"
        event += ", 啟用台電移頻指令:"+ str(mode_data['FP_main_f_set_source'])
        event += ", FP1:("+mode_data['f1_line_set']+","+mode_data['p1_line_set']+")"
        event += ", FP2:("+mode_data['f2_line_set']+","+mode_data['p2_line_set']+")"
        event += ", FP3:("+mode_data['f3_line_set']+","+mode_data['p3_line_set']+")"
        event += ", FP4:("+mode_data['f4_line_set']+","+mode_data['p4_line_set']+")"
        event += ", FP5:("+mode_data['f5_line_set']+","+mode_data['p5_line_set']+")"
        event += ", FP6:("+mode_data['f6_line_set']+","+mode_data['p6_line_set']+")"
        event += ", 電壓參考值:"+mode_data['Vq_v_base']+"kV, 虛功參考值:"+mode_data['Vq_q_base']+"kVAR"
        event += ", VQ1:("+mode_data['Vq_v1_set']+","+mode_data['Vq_q1_set']+")"
        event += ", VQ2:("+mode_data['Vq_v2_set']+","+mode_data['Vq_q2_set']+")"
        event += ", VQ3:("+mode_data['Vq_v3_set']+","+mode_data['Vq_q3_set']+")"
        event += ", VQ4:("+mode_data['Vq_v4_set']+","+mode_data['Vq_q4_set']+")"
        event += ", VQ5:("+mode_data['Vq_v5_set']+","+mode_data['Vq_q5_set']+")"
        event += ", VQ6:("+mode_data['Vq_v6_set']+","+mode_data['Vq_q6_set']+")"
        event += ", VQ7:("+mode_data['Vq_v7_set']+","+mode_data['Vq_q7_set']+")"
        event += " )"
      elif(mode_data['mode']=='schedule'):
        mode_data_old['mode'] = 0
        mode_data_old['scheduleFlag'] = 1
        event = "設定系統模式參數(模式:排程模式-停止模式-初始化)"
    status = "下達指令成功"
    db.site_control.insert(mode_data_old)
    c.SOE(db,"site_control",event,"systemMode")
  
  return jsonify(status = status)
#------------------------------------------------------------------------------------------------------
# 比例設定 儲存
@app.route('/scale_set')
def scale_set():
  user,db = check_user()
  if(db == None):
    return logout() 
  scale_set = json.loads(request.args.get('scale_set', type=str))
  ID_name_dict = json.loads(request.args.get('ID_name_dict', type=str))
  mode_data_old = c.current_data(db,'site_control',"site_control")[0]
  now = datetime.datetime.now()
  mode_data_old['time'] = now
  
  event = "設定系統模式之每台PCS發電佔比("
  first = 0
  _scale_set = {}
  for key,value in scale_set.items():
    try:
      _scale_set[key] = float(value)
    except:
      status = "比例數值錯誤"
      return jsonify(status = status)
    if(first!=0):
      event += ","
    name = ID_name_dict[key]
    _event = name + ":" + str(value)
    event += _event
    first = first + 1
  event +=")"
  mode_data_old["scale"] = _scale_set
  status = "下達指令成功"
  print(mode_data_old)
  db.site_control.insert(mode_data_old)
  c.SOE(db,"site_control",event,"systemMode")
  return jsonify(status = status)
#------------------------------------------------------------------------------------------------------
#ess set設定
@app.route('/ess_set')
def ess_set():
  user,db = check_user()
  if(db == None):
    return logout() 
  set_data = json.loads(request.args.get('set_data', type=str))
  equipType = request.args.get('equipType', type=str)
  now = datetime.datetime.now()
  sys_set = c.current_data(db,'sys_control','system')[0]
  protection = sys_set.get("protection",1)
  status = ""
  print(set_data)
  if(equipType=='pcs'):
    bmsOnOff = True
    if set_data['control']==1:
      connects = db.equipment.find_one({'_id': ObjectId(set_data['ID'])}, {'_id':0, 'connect':1})
      if "connect" in connects:
        for connect in connects["connect"]:
          control = c.current_data(db,"mbms_control", connect)[0]
          if control == {}:       # 此架構為一個mbms下連接兩個pcs, added by柯柯
            try:
              connect = list(db.equipment.find({"connect":set_data['ID']}))[0]
              connect = str(connect.get("_id"))
              control = c.current_data(db,"mbms_control", connect)[0]
              print("control:", control)
            except:
              pass
          bmsOnOff &= (control["control"] if "control" in control else False)
    if((protection==1 or protection==2) and set_data['control']==1):
      c.SOE(db,set_data["ID"],"設定PCS控制參數失敗,原因為系統保護中","equipControl")
      status = "系統保護中"
    elif not bmsOnOff:
      c.SOE(db,set_data["ID"],"設定PCS控制參數失敗,原因為有電池未搭接","equipControl")
      status = "有電池未搭接"
    else:
      statusTable = {0:"關閉",1:"開啟"}
      event = "設定PCS控制參數(狀態:"+str(statusTable[set_data['control']])+", 復歸:"+str(set_data['reset'])+")"
      set_data['time'] = now
      db.pcs_control.insert(set_data)
      c.SOE(db,set_data["ID"],event,"equipControl")
      status ="下達指令成功"
  elif(equipType=='mbms'):
    pcsOnOff = False
    if set_data['reset']==0:
      connects = db.equipment.find_one({'_id': ObjectId(set_data['ID'])}, {'_id':0, 'connect':1})
      if "connect" in connects:
        for connect in connects["connect"]:
          control = c.current_data(db,"pcs_control", connect)[0]
          pcsOnOff |= (control["control"] if "control" in control else True)
    if((protection==1 or protection==2) and set_data['control']==1):
      c.SOE(db,set_data["ID"],"設定MBMS控制參數失敗,原因為系統保護中","equipControl")
      status = "系統保護中"
    elif pcsOnOff:
      c.SOE(db,set_data["ID"],"設定MBMS控制參數失敗,原因為有PCS未停止","equipControl")
      status = "有PCS未停止"
    else:
      statusTable = {0:"關閉",1:"開啟"}
      event = "設定電池控制參數(狀態:"+str(statusTable[set_data['control']])+", 復歸:"+str(set_data['reset'])+")"
      set_data['time'] = now
      db.mbms_control.insert(set_data)
      c.SOE(db,set_data["ID"],event,"equipControl")
      status ="下達指令成功"
  elif(equipType=='air'):
    statusTable = {0:"關閉",1:"開啟"}
    event = "設定空調控制參數(狀態:"+str(statusTable[set_data['control']])+")"
    set_data['time'] = now
    db.air_control.insert(set_data)
    c.SOE(db,set_data["ID"],event,"equipControl")
           
  return jsonify(status = status)
#--------------------------------------------------------------------------------------------------
# 取得排程事件
@app.route('/get_event_in_schedule')
def get_event_in_schedule():
  user,db = check_user()
  if(db == None):
    return logout() 
  right = request.args.get('right',type=str)
  starttime = request.args.get('starttime',type=str)
  endtime = request.args.get('endtime',type=str)
  eventOrder = request.args.get('eventOrder',type=int)

  starttime = datetime.datetime.strptime(starttime,"%Y-%m-%d %H:%M:%S")
  endtime = datetime.datetime.strptime(endtime,"%Y-%m-%d %H:%M:%S")
  schedule = []
  modeLabel = {0:"停止模式",1:"1.固定功因模式",2:"2.電壓虛功模式",3:"3.電壓實功模式",4:"4.電壓實虛功模式",5:"5.頻率實功模式",6:"實虛功控制模式",7:"7.穩定實功模式",8:"8.平滑化模式",9:"9.功率逆送模式",10:"10.SOC校正模式" ,11:"AFC模式"}           
  FP_typeLable = {0:"dReg0.5",1:"dReg0.25",2:"自訂",3:"sReg"}
  search = {}
    
  if right == "EMS" or right == "USER" or right == "TPC":
    search = {'right':right,'start': {'$gte': starttime,'$lt':endtime},'show':1}
  elif right == " ":
    search = {'start': {'$gte': starttime,'$lt':endtime},'show':1}
  else:
    return jsonify(schedule=[])
  if eventOrder!=None:
    search["eventOrder"] = eventOrder

  print(search)
  for i in db.schedule.find(search):
  # for i in db.schedule.find(search,{"start":1,"end":1,"time":1,"mode":1,"eventOrder":1,"right":1,"FP_type":1,"FP_main_f_set":1}):
    i["_id"] = str(i["_id"])
    try:
      if i["mode"] == 0 and i["eventOrder"] == 0 and i["right"] == "USER":  
        i["title"] = "提出中止待命"
      elif i["mode"] == 0 and i["eventOrder"] == 0 and i["right"] == "EMS":
        i["title"] = "中止待命"
      elif i["mode"] == 11:  
        i["title"] = modeLabel[i["mode"]]+"("+FP_typeLable[i["FP_type"]]+"-"+str(i["FP_main_f_set"])+")"
      elif i["mode"] == [5,11] and i["right"] == "TPC" and i["eventOrder"] == 2 and "FP_main_f_set" in i :
        i["title"] = "移頻指令("+str(i["FP_main_f_set"])+")"
      else:
        i["title"] = modeLabel[i["mode"]]
    except:
        i["title"] = i["mode"]
    i["time"] = datetime.datetime.strftime(i["time"],"%Y-%m-%d %H:%M:%S")
    if "end" in i:
      i["end"] = datetime.datetime.strftime(i["end"],"%Y-%m-%d %H:%M:%S")
    i["start"] = datetime.datetime.strftime(i["start"],"%Y-%m-%d %H:%M:%S")
    # i.pop('right')
    # i.pop('FP_type')
    # i.pop('FP_main_f_set')
    schedule.append(i)
  return jsonify(schedule=schedule)
#------------------------------------------------------------------------------------------------------
# 取得最新排程事件
@app.route('/get_latest_event_in_schedule')
def get_latest_event_in_schedule():
  user,db = check_user()
  if(db == None):
    return logout() 
  right = request.args.get('right',type=str)
 
  search = {}
  if right!=" ":
    search = {'right':right,'show':1}
  else:
    search = {'show':1}

  event =  db.schedule.find_one(search, sort=[("start", -1)])
  if event == None: # 若未增加入事件，以排程的參考模式控制作為事件參考
    event =  db.schedule.find_one({'right':"REFERENCE",'show':0}, sort=[("start", -1)])
  if event == None: # 若未增加入事件，以最新的模式控制作為事件參考
    event = c.current_data(db,'site_control',"site_control")[0]
  try:
    event.pop("_id")
  except:
    pass
  print(event)
  return jsonify(event=event)
#------------------------------------------------------------------------------------------------------
# 新增排程事件
@app.route('/insert_event_in_schedule')
def insert_event_in_schedule():
  user,db = check_user()
  if(db == None):
    return logout() 
  eventTimeList = request.args.get('eventTimeList',type=str).split(",")
  mode_data = json.loads(request.args.get('mode_data', type=str))
  print(mode_data)
  now = datetime.datetime.now()
  status = "失敗"

  mode_data["time"] = now
  mode_data["show"] = 1
  mode_data["eventOrder"] = 1
  keyList = ['mode',
      'soc_max','soc_min','System_p_max','System_p_min','System_q_max','System_q_min',
      'Vq_v1_set','Vq_v2_set','Vq_v3_set','Vq_v4_set','Vq_v5_set','Vq_v6_set','Vq_v7_set','Vq_q1_set','Vq_q2_set','Vq_q3_set','Vq_q4_set','Vq_q5_set','Vq_q6_set','Vq_q7_set','Vq_q_base','Vq_v_base',
      'f1_line_set','f2_line_set','f3_line_set','f4_line_set','f5_line_set','f6_line_set','p1_line_set','p2_line_set','p3_line_set','p4_line_set','p5_line_set','p6_line_set',
      'FP_line_p_base','FP_type','FP_soc_goal_percent','FP_main_f_set',
      'PQ_p_ref','PQ_q_ref']
  for i in keyList:
    mode_data[i] = c.tonum(mode_data[i])
  
  for i in eventTimeList:
    try:
      mode_data.pop("_id")  
    except:
      pass
    starttime = datetime.datetime.strptime(i,"%Y-%m-%d %H:%M:%S")
    mode_data['start'] = starttime
    endtime = starttime + relativedelta(hours=1)
    mode_data['end'] = endtime
    db.schedule.insert(mode_data)
  
  status = "完成"
  return jsonify(status=status)
#------------------------------------------------------------------------------------------------------
# 移除排程事件
@app.route('/remove_event_in_schedule')
def remove_event_in_schedule():
  user,db = check_user()
  if(db == None):
    return logout() 
  eventIDList = request.args.get('eventIDList',type=str).split(",")
  # print(eventIDList)
  now = datetime.datetime.now()
  for eventID in eventIDList:
    db.schedule.update({"_id":ObjectId(eventID)},{"$set":{'show':0}}) 
  status = "完成"
  return jsonify(status=status)
#------------------------------------------------------------------------------------------------------
# 提出中止待命排程事件
@app.route('/insert_stopEvent_in_today_schedule')
def insert_stopEvent_in_today_schedule():
  user,db = check_user()
  if(db == None):
    return logout() 

  now = datetime.datetime.now() + relativedelta(second=0,microsecond=0)
  # starttime = now + relativedelta(hours=1,minutes=30,second=0,microsecond=0)
  starttime = now + relativedelta(minutes=1,second=0,microsecond=0)
  endtime = starttime  + relativedelta(days=1,hour=0,minute=0,second=0,microsecond=0)

  event =  db.schedule.find_one({'right':"USER",'show':1}, sort=[("start", -1)])
  if event == None: # 若未增加入事件，以排程的參考模式控制作為事件參考
    event =  db.schedule.find_one({'right':"REFERENCE",'show':0}, sort=[("start", -1)])
  if event == None: # 若未增加入事件，以最新的模式控制作為事件參考
    event = c.current_data(db,'site_control',"site_control")[0]
  try:
    event.pop("_id")
  except:
    pass

  scheduleEPOEvent =  db.schedule.find_one({"ID":"schedule","start":{"$lte":now},"end":{"$gt":now},"eventOrder":0,"show":1})
  if scheduleEPOEvent != None:
    status = "已提出中止待命，請勿再次提出"
  else:
    # 取消後續排程事件
    db.schedule.update_many({"start":{'$gte': starttime,'$lt':endtime}},{"$set":{"show":0}})
    print(event)
    # event基本參數
    event["ID"]  = "schedule"
    event["eventOrder"] = 0
    event["show"] = 1
    # 開始中止待命至結束中止待命區間
    event["right"] = "EMS"
    event["start"] = starttime
    event["end"] = endtime
    event["mode"] = 0
    db.schedule.insert(event)
    # 提出中止待命至開始中止待命區間
    event["right"] = "USER"
    event["start"] = now
    event["end"] = starttime
    event["command_id"] = str(event.pop("_id"))
    db.schedule.insert(event)
    status = "完成"
    
  return jsonify(status=status)
#------------------------------------------------------------------------------------------------------
# 停止中止待命排程事件
@app.route('/stop_stopEvent_in_today_schedule')
def stop_stopEvent_in_today_schedule():
  user,db = check_user()
  if(db == None):
    return logout() 

  eventID = request.args.get('eventID',type=str)
  start = request.args.get('start',type=str)
  start = datetime.datetime.strptime(start,"%Y-%m-%d %H:%M:%S")

  now = datetime.datetime.now()
  if(start<now):
    db.schedule.update({"_id":ObjectId(eventID)},{"$set":{"end":now,"stopFlag":1}})
    status = "完成"
  else:
    status = "系統暫無法停止中止待命，若要移除此事件，請取消已提出的中止命令"
  return jsonify(status=status)
#------------------------------------------------------------------------------------------------------
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
#告警確認
@app.route('/alarm_check')
def alarm_check():
  user,db = check_user()
  if(db == None):
    return logout() 
  event_id = request.args.get('event_id', type=str).split(',')
  now = datetime.datetime.now()
  for i in event_id:
    db.alarm.update({'_id':ObjectId(i),'checktime':None},{'$set':{'checktime':now}})
  return jsonify()
#--------------------------------------------------------------------------------------------------
#告警歸檔
@app.route('/alarm_save')
def alarm_save():
  user,db = check_user()
  if(db == None):
    return logout() 
  event_id = request.args.get('event_id', type=str).split(',')
  for i in event_id:
    db.alarm.update({'_id':ObjectId(i)},{'$set':{'show':2}})
  return jsonify()
#--------------------------------------------------------------------------------------------------
#告警刪除
@app.route('/alarm_delete')
def alarm_delete():
  user,db = check_user()
  if(db == None):
    return logout() 
  event_id = request.args.get('event_id', type=str).split(',')
  for i in event_id:
    db.alarm.update({'_id':ObjectId(i)},{'$set':{'show':0}})
  return jsonify()
#--------------------------------------------------------------------------------------------------
#取得 EMS 伺服器和資料庫狀態
@app.route('/get_ems_pc_info_data')
def get_ems_pc_info_data():
  user,db = check_user()  
  if(db == None):
    return logout() 
  pc_list = ["ems1", "ems2", "gc1", "gc2", "mbms", "dc", "hmi", "sdc"]
  pc_status = {}
  for pc in pc_list:
    try:
      pc_status[pc] = c.current_data(db,'pc_info',pc,errortime=300)[0]
    except:
      pc_status[pc] = {}
    pc_status[pc]['time'] = c.time2string(pc_status[pc].get("time",None))
    pc_status[pc]['boot_time'] = c.time2string(pc_status[pc].get("boot_time",None))
  # pc_status["ems1"] = pc_status["peter-H370HD3"]
  return jsonify(data=pc_status)
#--------------------------------------------------------------------------------------------------
#取得 program 狀態
@app.route('/get_program_data')
def get_program_data():
  user,db = check_user()  
  if(db == None):
    return logout() 
  data = list(db.program.find({},{"_id":0}))
  return jsonify(data=data)
#--------------------------------------------------------------------------------------------------
#取得 program 狀態
@app.route('/get_equipment_status_data')
def get_equipment_status_data():
  user,db = check_user()  
  if(db == None):
    return logout() 
  data = list(db.equipment_status.find({},{"_id":0}))
  return jsonify(data=data)
#--------------------------------------------------------------------------------------------------
@app.route('/soe_get')
def soe_get():
  user,db = check_user()  
  if(db == None):
    return logout() 
  limit = request.args.get('limit',type=int)
  start = request.args.get('start',type=int)
  page = request.args.get('page',type=int)
  datepicker1 = request.args.get('datepicker1', type=str)
  datepicker2 = request.args.get('datepicker2', type=str)
  search = request.args.get('search', type=int)
  soeType = request.args.get('soeType', type=str)
  skip = page * limit
  # print(limit,start,page)
  data=[]
  total = 0

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
  soeType_condiction = {}
  if(soeType!="all"):
    soeType_condiction["type"] = soeType
  equip_list = list(db.equipment.find({}))
  program_list = list(db.program.find({}))

  total = db.SOE.find({**time_condiction,**soeType_condiction},{'_id':1}).count()
  for i in db.SOE.aggregate([
    {'$match':{**time_condiction,**soeType_condiction}},
    {'$sort':{"time":-1,"_id":-1}},
    {'$skip':skip},
    {'$limit':limit},
    {'$project': {
      '_id': {'$toString': "$_id"},
      'ID':1,
      'event':1,
      'type':1,
      'typeText':{
            '$switch': {
              'branches': [
                { 'case': { '$eq': [  "$type", "system" ] }, 'then': "系統相關" },
                { 'case': { '$eq': [  "$type", "systemMode" ] }, 'then': "系統模式" },
                { 'case': { '$eq': [  "$type", "equipControl" ] }, 'then': "設備控制" },
                { 'case': { '$eq': [  "$type", "protection" ] }, 'then': "保護邏輯" },
                { 'case': { '$eq': [  "$type", "status" ] }, 'then': "狀態" },
                { 'case': { '$eq': [  "$type", "access" ] }, 'then': "門禁" },
                { 'case': { '$eq': [  "$type", "other" ] }, 'then': "其他" },
              ],
              'default': "Fault"
            }
          },
      'time': { '$dateToString': { 'date': "$time",'format': "%Y-%m-%d %H:%M:%S" ,'onNull': ''} },
    }},
  ]):
    start = start+1
    i['index'] = start
    i['item'] = None
    i['place'] = None
    if(i['ID'] == "system"):
      i['item'] = "系統相關"
      i['place'] = None
    elif(i['ID'] == "site_control"):
      i['item'] = "系統模式"
      i['place'] = None
    else:
      index = next((index for index,equip in enumerate(equip_list) if str(equip["_id"]) == i['ID']), None)
      # print(index)
      index2 = None
      if(index != None):
        i['item'] = equip_list[index]['name']
        i['place'] = equip_list[index]['place']
        index = None
      else:
        index2 = next((index for index,program in enumerate(program_list) if str(program["ID"]) == i['ID']), None) 
      if(index2 != None):
        i['item'] = program_list[index2]['name']
        i['place'] = program_list[index2]['place']
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
@app.route('/control')
@login_required
def control():
  user,db = check_user()
  if(db == None):
    return logout() 
  equipType = request.args.get('equipType',type=str)
  for user in db.users.find({'username':user[1]}):
    username = user['username']
    level = user['level']
  return render_template('control.html',username=username,level=level,equipType=equipType)
#--------------------------------------------------------------------------------------------------
@app.route('/status')
@login_required
def status():
  user,db = check_user()
  if(db == None):
    return logout() 
  for user in db.users.find({'username':user[1]}):
    username = user['username']
    level = user['level']
  return render_template('status.html',username=username,level=level)
#--------------------------------------------------------------------------------------------------
@app.route('/alarm')
@login_required
def alarm():
  user,db = check_user()
  if(db == None):
    return logout() 
  for user in db.users.find({'username':user[1]}):
    username = user['username']
    level = user['level']
  return render_template('alarm.html',username=username,level=level)
#--------------------------------------------------------------------------------------------------
@app.route('/soe')
@login_required
def soe():
  user,db = check_user()
  if(db == None):
    return logout() 
  for user in db.users.find({'username':user[1]}):
    username = user['username']
    level = user['level']
  return render_template('SOE.html',username=username,level=level)
#--------------------------------------------------------------------------------------------------
#==================================工程頁面撈資料edited by睿彬-=====================================
@app.route('/get_history_plot') 
def get_history_plot():
    # db = conn[ 'school' ]
    # db1 = conn['delta']

    user,db = check_user()
    if(db == None):
      return logout() 
    interval = 1
    y_axis = []
    #-------------------------------接收輸入資料 ----------------------------
    datepickerstart = request.args.get('datepickerstart',type = str)
    starttime = datetime.datetime.strptime(datepickerstart, "%Y-%m-%d")
    content_list = eval( request.args.get('list',type= str) )
    #-------------------------------傳送預設資料到mongodb-------------------- 
    # plot_one_day.save_pre_data(db=db,collection='pre_data',content_list=content_list)
    #-----------------------------------------------------------------------
    x_axis = plot_one_day.x_one_day_line(starttime) # 生成X軸(區間為一天)
    for content in content_list:
        if(content != []):
            data = plot_one_day.y_one_day_line(
                db=db,
                collection=content[0],
                ID=content[1],
                datatype=content[2],
                date=starttime,
                interval=interval
            )
            y_axis.append(data)
        else:
            y_axis.append([])
    return jsonify(x_axis=x_axis,y_axis=y_axis) #edit by 睿彬
#==================================================================================================
#================================歷史曲線匯出CSV by睿彬=============================================
@app.route('/history_csv_OneDay')
def history_csv_one_day():
    user,db = check_user()
    if(db == None):
      return logout() 
    try:
        # db0 = conn[ 'school' ]
        # db1 = conn['delta']
        data_list = []
        csvList = []
        #===第一行為時間========#
        data_tag = ['Date/Time']
        #====================#
        content_list = eval(request.args.get('list', type=str))   #contain [[col_name, device_id, prepare_data], [...], ...]
        date = request.args.get('date', type=str)
        starttime = datetime.datetime.strptime(date+' 00:00:00', "%Y-%m-%d %H:%M:%S")
        endtime = starttime + datetime.timedelta(days=1)
        Time = plot_one_day.time_interval(starttime=starttime,endtime=endtime)
        for num,content in enumerate(content_list):
            data = plot_one_day.get_data(db=db,collection=content[0],ID=content[1],key=content[2],starttime=starttime,endtime=endtime )
            data_list.append(data)
            data_tag.append( content[0] + ' / ' + content[1] + ' / ' + content[2])
        csvList.append(data_tag)
        for i,time in enumerate(Time): 
            row = []
            row.append(time)
            for index in range(0,len(data_list)):
                row.append(data_list[index][i])
            csvList.append(row)

        si = io.StringIO()
        cw = csv.writer(si)
        cw.writerows(csvList)

        mem = io.BytesIO()
        mem.write(si.getvalue().encode('utf-8-sig'))
        mem.seek(0)
        si.close()
    except (AttributeError,TypeError,Exception) as e:
        print(e)
        return jsonify(result = 0 ,error = str(e) )
    else:
        return send_file(
        mem,
        as_attachment=True,
        attachment_filename= date+'_history.csv',
        mimetype='text/csv'
        )
# #==================================================================================================
# #====================================網址輸入時間的模式曲線==========================================
# @app.route('/modeChart/<time>')      #有改動 3/24 by 睿彬 #還不能輸入容量
# @login_required
# def modeChartbyurl(time):
#     user,db = check_user()
#     if(db == None):
#       return logout() 
#     try:
#         # db = conn['delta']
#         x_axis =[]
#         if ( len(time) >= 22 ):
#             starttime = datetime.datetime.strptime(time[0:19],"%Y-%m-%d-%H:%M:%S")
#             x_len = int(time[20:])
#             nexttime = starttime + relativedelta(seconds=x_len)
#             p = t.user_set_secondline(db,"acm","c1_acm3","p",nexttime,interval=1,after_secs=x_len )
#             f = t.user_set_secondline(db,"acm","c1_acm3","f",nexttime,interval=1,after_secs=x_len )
#             ideal_p =t.user_set_secondline(db,"afc_ideal","c1_acm3","ideal_p",nexttime,interval=1,after_secs=x_len )
#             pm =caculate_pm(f,p)
#             pm_avg = avg_pm(pm[0]) #edit by 睿彬 3/23

#         else:                      #畫一小時的
#             time = time[0:13]
#             starttime = datetime.datetime.strptime(time,"%Y-%m-%d-%H")
#             x_len = int(86400*1/24)
#             nexttime = starttime + datetime.timedelta(hours=1) #資料在時間區段終點
#             p = t.hourline_one(db,"acm","c1_acm3","p",nexttime,interval=1)
#             f = t.hourline_one(db,"acm","c1_acm3","f",nexttime,interval=1)
#             ideal_p =t.hourline_one(db,"afc_ideal","c1_acm3","ideal_p",nexttime,interval=1)
#             pm =caculate_pm(f,p)
            
#             pm_avg = avg_pm(pm[0]) #edit by 睿彬 3/23
            
#         for i in range(x_len):
#             x_axis.append(datetime.datetime.strftime(starttime,"%H:%M:%S.%f"))
#             starttime  = starttime + relativedelta(microseconds=1000000)
#     except (AttributeError,TypeError) as error_message:
#         return render_template('modeChartbyurl.html' , data={'x': [],'y': [],'pm_avg':0,'error_message':str(error_message)})
#     else:
#         fix1 =[60.02]*3600#*2
#         fix2 =[59.98]*3600#*2
#         fix3 =[59.75]*3600#*2
#         fix4 =[60.25]*3600#*2
#         y_axis = [p,ideal_p,pm[0],f,fix1,fix2,fix3,fix4]
#         # y_axis = [p,f,fix1,fix2,fix3,fix4]
#         return render_template('modeChartbyurl.html' , data={'x': x_axis,'y': y_axis,'pm_avg':pm_avg})
# #==================================================================================================
# @app.route('/history') # 新增工程頁面 by 睿彬
# @login_required
# def history():
#   user,db = check_user()
#   if(db == None):
#     return logout() 
#   # db = conn['school'] 
#   collection = db['pre_data']
#   pre_data = collection.find_one()
#   for user in db.users.find({'username':user[1]}):
#     username = user['username']
#     level = user['level']
#     # print(pre_data)
#   return render_template('history.html',pre_data=pre_data,username=username,level=level)
# #--------------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------
#報表
@app.route('/report')
@login_required
def report():
  user,db = check_user()
  if(db == None):
    return logout() 
  for user in db.users.find({'username':user[1]}):
    username = user['username']
    level = user['level']
  return render_template('report.html',username=username,level=level)

  #--------------------------------------------------------------------------------------------------
@app.route('/report_get')
def report_get():
  user,db = check_user()  
  if(db == None):
    return logout() 
  limit = request.args.get('limit',type=int)
  start = request.args.get('start',type=int)
  page = request.args.get('page',type=int)
  datepicker1 = request.args.get('datepicker1', type=str)
  datepicker2 = request.args.get('datepicker2', type=str)
  search = request.args.get('search', type=int)
  reportType = request.args.get('reportType', type=str)
  skip = page * limit
  # print(limit,start,page)
  data=[]
  total = 0

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
  reportType_condiction = {}
  if(reportType!="all"):
    reportType_condiction["type"] = reportType
  print("report" ,time_condiction,reportType_condiction)
  total = db.report.find({**time_condiction,**reportType_condiction},{'_id':1}).count()

  for i in db.report.aggregate([
    {'$match':{**time_condiction,**reportType_condiction}},

    {'$sort':{"time":-1,"_id":-1}},
    {'$skip':skip},
    {'$limit':limit},
    {'$project': {
      '_id': {'$toString': "$_id"},
      'FileName':1,
      'type':1,

      'time': { '$dateToString': { 'date': "$time",'format': "%Y-%m-%d %H:%M:%S" ,'onNull': ''}} },
    },
  ]):
    start = start+1
    i['index'] = start
    data.append(i)
  return jsonify(data=data, total=total)
@app.route('/downloadReport', methods=['GET'])
@login_required
def downloadReport():
  user,db = check_user()  
  if(db == None):
    return logout() 
  FileName = request.args.get('FileName',type=str)

  file=db.report.find_one({"FileName":FileName})

  response=send_file(
      io.BytesIO(file["file"]),
              attachment_filename=FileName+".pdf",
        as_attachment=True,

      mimetype='application/pdf'
  )


  return  response
#--------------------------------------------------------------------------------------------------
if __name__ == '__main__':
  app.run(host='0.0.0.0',port='8080')
  # app.run(host='127.0.0.1',port='8080') #test only#