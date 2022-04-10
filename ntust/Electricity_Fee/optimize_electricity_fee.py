import pandas as pd
from pandas import DataFrame, Series
import numpy as np
import docplex
from docplex.mp.model import Model

NOMb = 200            #標稱電池容量,單位為kWh
ESS_Ch_max = 50       #Battery max允許以kW為單位的充電功率
ESS_Disch_max = 50    #Battery max允許以kW為單位的放電功率，放電功率是正的 
SOC_Init = 0.5        #初始儲能電池SOC
SOCmin = 0.1          #電池充電狀態(最低SOC)
SOCmax =0.9           #電池充電狀態(最高SOC)
SOC_final =0.5        #儲能電池目標SOC
efficiency=0.97       #電池充放電效率都假設一樣 
MOF_efficiency = 0.98 #MOF點的效率(pcs以上上位裝置)

# 電力負載資料(loadprofile) 24小時(48個點)(每半小時一個點)
loadprofile=[130, 123, 116, 110, 105, 100, 105, 110, 123, 140, 159, 172, 135, 172, 168, 172, 167, 155, 150, 145, 140, 142, 140, 135,
             130, 123, 116, 110, 105, 100, 105, 110, 123, 140, 159, 172, 135, 172, 168, 172, 167, 155, 150, 145, 140, 142, 140, 135]
# 時間戳   24小時
# Index=      [000,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000,11000,1200,1300,1400,1500,1600,1700,1800,1900,2000,2100,2200,2300]
# 時間電價 24小時(48個點)(每半小時一個點)
TOU_index = [1.32, 1.32, 1.32, 1.32, 1.32, 1.32, 1.32, 1.32, 2.9, 2.9, 4.67, 4.67, 2.9, 1.32, 4.67, 4.67, 4.67, 2.9, 2.9, 2.9, 2.9, 2.9, 1.32, 1,
             1.32, 1.32, 1.32, 1.32, 1.32, 1.32, 1.32, 1.32, 2.9, 2.9, 4.67, 4.67, 2.9, 1.32, 4.67, 4.67, 4.67, 2.9, 2.9, 2.9, 2.9, 2.9, 1.32, 1]

#問題模型選擇經濟調度優化問題 The Unit Commitment Problem (UCP)
ucpm = Model("ucp") 
loadprofile= Series(loadprofile)
nb_periods = len(loadprofile)
sche_lengh = len(loadprofile)
demand = loadprofile
# 設定設備模型
energies = ["coal", "gas", "diesel", "wind"]
df_energy = DataFrame({"co2_cost": [30, 5, 15, 0]}, index=energies)

all_units = ["diesel1"]
ucp_raw_unit_data = {
        "energy": ["diesel"],
        "initial" : [demand[0]],
        }
        
ess_index = ["ess1"]
ucp_raw_ess_data = {
        "energy": ["ess"],
        "initial" : [0],
        "max_ch": [ESS_Ch_max],
        "max_disch": [ESS_Disch_max],
        }
df_units = DataFrame(ucp_raw_unit_data, index=all_units)
ess_unit = DataFrame(ucp_raw_ess_data, index=ess_index)
df_up = pd.merge(df_units, df_energy, left_on="energy", right_index=True)
df_up.index.names=['units'] 
df_units.index.names=['df_unit'] 
ess_unit.index.names=['ess_unit'] 
# print("------------------------------------")
# print("df_energy:")
# print(df_energy)
# print("------------------------------------")
# print("df_units:")
# print(df_units)
# print("------------------------------------")
# print("ess_unit:")
# print(ess_unit)
# print("------------------------------------")
# print("df_up:")
# print(df_up)
# ------------------------------------------------------------------
# 設定決策變量(decision variables)
# 設備列表 
ess = ["ess1"]
# 時間長度 0~48 range from 1 to nb_periods included 
periods = range(0, nb_periods)
production = ucpm.continuous_var_matrix(keys1=all_units, keys2=periods, name="p")
production_variation = ucpm.continuous_var_matrix(keys1=all_units, keys2=periods, name="production_variation")

# 同一個時間只能充電或是放電 
charge_var = ucpm.binary_var_matrix(keys1=ess, keys2=periods, name="charge_var")
discharge_var = ucpm.binary_var_matrix(keys1=ess, keys2=periods, name="discharge_var")
# 儲能系統充放電功率(PCS)
ess_ch_production = ucpm.continuous_var_matrix(keys1=ess, keys2=periods, name="ess_ch_production")
ess_disch_production = ucpm.continuous_var_matrix(keys1=ess, keys2=periods, name="ess_disch_production")
# 儲能電池SOC
ess_soc = ucpm.continuous_var_matrix(keys1=ess, keys2=periods, name="ess_soc")
# 把所有的優化變數在整合成一個表格，每個基礎每個時間的優化變數  
df_decision_vars = DataFrame({'production': production,'production_variation': production_variation })
df_decision_vars_ess = DataFrame({'charge_var': charge_var,'discharge_var' : discharge_var,'ess_ch_production' : ess_ch_production ,'ess_disch_production' : ess_disch_production,'ess_soc':ess_soc})
# Set index names (增加兩個index，機組名稱與時間)
df_decision_vars.index.names=['units', 'periods']
df_decision_vars_ess.index.names=['ess_unit', 'periods']
# 把儲能系統上下功率限制 黏貼過來 
df_join_decision_vars_ess_minmax = df_decision_vars_ess.join(ess_unit[['max_ch', 'max_disch']], how='inner')
# ------------------------------------------------------------------
print("------------------------------------")
print("df_decision_vars:")
print(df_decision_vars)
print("------------------------------------")
print("df_join_decision_vars_ess_minmax:")
print(df_join_decision_vars_ess_minmax)
# ------------------------------------------------------------------
# 設定約束條件
# 儲能充放電功率、SOC的限制，要在最大到最小之間，疊代每個列(橫)，也就是疊代每一個機組的每一個時間點
for item in df_join_decision_vars_ess_minmax.itertuples(index=False):
    ucpm += (item.ess_disch_production <= item.max_disch * item.discharge_var) #放電功率 <= 最大放電功率*放電參數(0 or 1，同時間只會充電或是放電)
    ucpm += (item.ess_disch_production >= 0)                                   #放電功率 >= 0
    ucpm += (item.ess_ch_production <= item.max_ch * item.charge_var)          #充電功率 <= 最大充電功率*充電參數(0 or 1，同時間只會充電或是放電)
    ucpm += (item.ess_ch_production >= 0)                                      #充電功率 >= 0
    ucpm += (item.ess_soc >= SOCmin)                                           #儲能電池SOC不能低於限制SOC之最小值
    ucpm += (item.ess_soc <= SOCmax)                                           #儲能電池SOC不能高於限制SOC之最大值
    ucpm += (item.charge_var + item.discharge_var <= 1 )                       #同時間只會充電或是放電 

# soc變動限制，現在的電量會等於上個時刻的電量，加上功率流動 ，而這個小時的soc則是這個小時的功率流動完之後的結果
# 對於不同的幾組設定不同的限制，以groupby來分類決策(優化)變數們(分類依據為ess_unit)，而這邊只有一個單位(機組)(ess1)，因此最外圈的for只會跑一次
# 因為 Q = 電量 (單位kwh) 且當前程式資料區間是每半小時一點，因此在列限制式時要除2，才能算出每半小時的電量流動
for ess_unit, r in df_decision_vars_ess.groupby(level='ess_unit'): 
    # 第0個時間點的電量Q = 初始電量Q(NOMB*SOC_Initial) + 第0個時間點的電量流動delta Q(Q單位為kwh)
    ucpm.add_constraint(NOMb*SOC_Init - NOMb* r.ess_soc[0]  + (r.ess_ch_production[0] * efficiency/(sche_lengh/24)) - (r.ess_disch_production[0] / efficiency/(sche_lengh/24)) == 0) #初始化 
    # 當前時間點的電量Q = 上一個時間點的電量Q  + 當前時間點(上個時間點到當前時間點)的電量流動 
    for (p_ch_curr, p_disch_curr, soc_curr, soc_next) in zip(r.ess_ch_production[1:],r.ess_disch_production[1:],r.ess_soc, r.ess_soc[1:]): #從第二個到最後一個 
        ucpm.add_constraint(NOMb*soc_curr - NOMb*soc_next + p_ch_curr * efficiency/(sche_lengh/24) - p_disch_curr / efficiency /(sche_lengh/24) == 0)
        #效率只能假設一個   
ucpm.add_constraint(ess_soc['ess1',len(loadprofile)-1] == SOC_final)

# 電力供需平衡，功率變動量計算 
for period, r in df_decision_vars.groupby(level='periods'):   
    total_demand = demand[period] #period 1 與load 1 相同
#   用來命名約束式的字串(可選的)
    ctname = "ct_meet_demand_%d" % period 
#   發電機發電功率+儲能放電功率*效率-儲能充電功率/效率
#   這邊充電效率儲的原因是充電視為增加需量，若一樣是乘以效率，這樣發電機發出的電就會減少，但事實上發電機發出的電需要多，要不然供不應求(儲能充電時)
#   所有機組的發電再加上儲能系統輸出功率 >= 負載功率需求
    ucpm.add_constraint(
        r.production[0] + 
        df_decision_vars_ess.loc['ess1',period].ess_disch_production * MOF_efficiency    -
        df_decision_vars_ess.loc['ess1',period].ess_ch_production / MOF_efficiency   == total_demand, ctname)
#   production_variation 可直接等於發電機組提供的電*時間電價，然後將production_variation設為目標函數
#   因會透過上面的限制式(所有機組的發電再加上儲能系統輸出功率>= 負載功率需求)，當儲能輸出，就不需要發電機提供全部的功率來滿足負載 -> 儲能輸出與發電機輸出相反
    ucpm.add_constraint(df_decision_vars.loc['diesel1',period].production_variation == df_decision_vars.loc['diesel1',period].production*TOU_index[period]/(sche_lengh/24))
# ------------------------------------------------------------------

# ------------------------------------------------------------------
# 設定目標函數 求解
# 功率大小會影響變動成本
total_variable_cost = ucpm.sum(df_decision_vars.production_variation)  
ucpm.add_kpi(total_variable_cost, "Total Variable Cost")
ucpm.minimize(total_variable_cost)
ucpm.print_information()
print('---------------------------------------')

assert ucpm.solve(), "Solve of the model fails !!!"
ucpm.report()
print('---------------------------------------')
# ------------------------------------------------------------------

# ------------------------------------------------------------------
# 整合輸出並比較
df_p_diesel = df_decision_vars.production.apply(lambda v: max(0, v.solution_value)).unstack(level='units')
df_ess_disch_p =df_decision_vars_ess.ess_disch_production.apply(lambda v: max(0, v.solution_value)).unstack(level='ess_unit')
df_ess_ch_p =df_decision_vars_ess.ess_ch_production.apply(lambda v: max(0, v.solution_value)).unstack(level='ess_unit')
df_ess_soc =df_decision_vars_ess.ess_soc.apply(lambda v: max(0, v.solution_value)).unstack(level='ess_unit')
df_ess_charge_var = df_decision_vars_ess.charge_var.apply(lambda v : v.solution_value).unstack(level='ess_unit')
df_ess_discharge_var = df_decision_vars_ess.discharge_var.apply(lambda v : v.solution_value).unstack(level='ess_unit')
ess_power = df_ess_disch_p - df_ess_ch_p
final_load =demand.sub(ess_power['ess1'], fill_value=0)
p_var = [0]
for i in range(len(final_load)):
    if i>0:
        p_var.append(final_load[i]-final_load[i-1])
output = DataFrame({'original_load': demand,   #原本的負載需求
                    'ess': ess_power['ess1'], #儲能需量(充電為負，放電為正)
                    'new_load':final_load,    #加入儲能後，總需量
                    'Power_variation' :p_var, #功率變動(加入儲能後需量變動)
                    'SOC':df_ess_soc['ess1']*100,
                    'diesel_P':df_p_diesel['diesel1'],
                    'charge_var':df_ess_charge_var['ess1'],
                    'discharge_var':df_ess_discharge_var['ess1'],
                    'charge_p' : df_ess_ch_p['ess1'],
                    'discharge_p' : df_ess_disch_p['ess1'],
                    })

#原本成本
original_cost = 0 
for i in range(len(loadprofile)):
# 要除的原因是因為 TOU_index 裡的每個值得單位是"kwh"，但我們在算的時間間隔是半小時一點
# 元 = 元/kwh(單位電價) * 單位負載用量(半小時) * 0.5h(除2) 
    original_cost = original_cost+ loadprofile[i]*TOU_index[i]/( len(TOU_index)/24) 
ucpm.report_kpis()
print('original_cost :',original_cost)
print('---------------------------------------')
# print(output)

# 輸入mongo排程
import pymongo
import datetime
from dateutil.relativedelta import relativedelta

conn = pymongo.MongoClient('mongodb://root:PaSsw0rd1@127.0.0.1:27017/')
db = conn['test']
starttime = datetime.datetime.now() + relativedelta(days=1,hour=0,minute=0,second=0,microsecond=0)
schedule_data ={
    "ID":"schedule",
    "time":"",
    "start":"",
    "end":"",
    "right":"EMS",
    "eventOrder":1,
    "soc_max" : 90,
    "soc_min" : 10,
    "System_p_max" : 100,
    "System_p_min" : -100,
    "System_q_max" : 100,
    "System_q_min" : -100,
    "mode" : 6,
    "f1_line_set" : 59.5,
    "f2_line_set" : 59.75,
    "f3_line_set" : 59.98,
    "f4_line_set" : 60.02,
    "f5_line_set" : 60.25,
    "f6_line_set" : 60.5,
    "p1_line_set" : 100,
    "p2_line_set" : 48,
    "p3_line_set" : 0,
    "p4_line_set" : 0,
    "p5_line_set" : -48,
    "p6_line_set" : -100,
    "FP_line_p_base" : 1800,
    "FP_type" : 0,
    "FP_soc_goal_percent" : 50,
    "FP_main_f_set" : 60,
    "Vq_v1_set" : 95,
    "Vq_v2_set" : 98,
    "Vq_v3_set" : 103,
    "Vq_v4_set" : 105,
    "Vq_v5_set" : 102,
    "Vq_v6_set" : 97,
    "Vq_v7_set" : 100,
    "Vq_q1_set" : 100,
    "Vq_q2_set" : 80,
    "Vq_q3_set" : -80,
    "Vq_q4_set" : -100,
    "Vq_q5_set" : -80,
    "Vq_q6_set" : 80,
    "Vq_q7_set" : 100,
    "Vq_q_base" : 300,
    "Vq_v_base" : 11.4,
    "PQ_p_ref":100,
    "PQ_q_ref":0,
    "show": 1,
}
for row in output.itertuples():
    schedule_data['time'] = datetime.datetime.now()
    schedule_data['start'] = starttime
    schedule_data['end'] = starttime+relativedelta(minutes=30)
    schedule_data['PQ_p_ref'] = getattr(row, 'ess')
    starttime = starttime+relativedelta(minutes=30)
    db.site_control.insert(schedule_data)
    del schedule_data['_id']
