
#from os import startfile
from pathlib import Path
import sys
#from PIL.Image import NONE
#from dns.resolver import NoAnswer
from weasyprint import HTML
from bs4 import BeautifulSoup
import datetime
import pymongo
from dateutil.relativedelta import relativedelta
# from submain  import  GetLocalIPByPrefix
import os
import psutil
import math
import traceback
#----------------------------------------------------------------------------------------------------------------------------------------
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

conn = pymongo.MongoClient('mongodb://root:pc152@127.0.0.1:27017/',serverSelectionTimeoutMS=10)

db = conn['AFC']
_db = conn['admin']


# 月報, m=x,d=1
# 季報, m=4,7,10,1 ,d=1
# 年報, m=1 ,d=1

def genReport(mode,date=datetime.datetime.now() ):

    if(GetLocalIPByPrefix()!=0 or 1 ):
        genDate=date
        ###SET DATE RANGE
        print("start Report",mode,date)
        if mode == "month":
            end = datetime.datetime(year=date.year, month=date.month, day=1, hour=0, minute=0, second=0, microsecond=0)

            date = date-relativedelta(months=1)
            start=datetime.datetime(year=date.year, month=date.month, day=1, hour=0, minute=0, second=0, microsecond=0)
            rowStart=1
            rowEnd=31
            title="{}年度{}月份報表".format(date.year,date.month)
        elif mode == "quarter":
            end = datetime.datetime(year=date.year, month=date.month,day=1, hour=0, minute=0, second=0, microsecond=0)
            date = date-relativedelta(months=3)
            start = datetime.datetime(year=date.year, month=date.month, day=1, hour=0, minute=0, second=0, microsecond=0)
            rowStart=date.month
            rowEnd=date.month+2

            title="{}年度第{}季報表".format(date.year,(date.month//3)+1)

        elif mode == "year":
            start = datetime.datetime(year=date.year-1, month=1, day=1, hour=0, minute=0, second=0, microsecond=0)
            end = datetime.datetime(year=date.year, month=1, day=1, hour=0, minute=0, second=0, microsecond=0)
            rowStart=1
            rowEnd=12
            title="{}年度報表".format(date.year-1)

        """Generate a PDF file from a string of HTML."""

        dirpath=os.path.dirname(__file__)
        soup = BeautifulSoup(
            open(os.path.join(dirpath,"templates/reportTemplate.html"), encoding="utf-8"), 'html.parser')
        table = soup.find('tbody')
        
        
        soup.find(id="genDate").string='產出日期 : '+genDate.strftime('%Y{y}%m{m}%d{d}').format(y='年', m='月', d='日')



        soup.find(id="title").string=title



        for i in range(rowEnd,rowStart-1, -1):
            table.insert(6, soup.new_tag('tr', id="dataRow{}".format(i)))
            for x in range(8):

                td=soup.new_tag('td')
                td.append(soup.new_tag('span', attrs={"class": "dataCol"}))

                table.find(id="dataRow{}".format(i)).insert(
                    1, td)

                # print(table)

                table.find(id="dataRow{}".format(i)).find(
                    class_="dataCol").string = str(i)



        # 執行率
        match = {"time": {"$gte": start, "$lt": end}}
        project = {
                        "day": {"$dayOfMonth": "$time"},
                        "month": {"$month": "$time"},
                        "SPM": 1
                    }
        group = {"_id": "$day",
                "avg": {"$avg": '$SPM'}}

        if mode == "month":
            group["_id"]="$day"
        else:
            group["_id"]="$month"

        #### every day or month
        dataList = list(db.SPM.aggregate(   [
                {'$match': match},
                {"$project":project},  
                {'$group': group},
            ]))

        print(dataList)
        for data in dataList:
            table.find(id="dataRow{}".format(data["_id"])).find_all(
                class_="dataCol")[1].string = str(round(data["avg"]*100, 1))

        #### AVG
        group["_id"]=None
        dataList = list(db.SPM.aggregate(   [
                    {'$match': match},
                    {"$project":project},  
                    {'$group': group},
                ]))
        print(dataList)
        for data in dataList:
            
            table.find(id="SPM").string = str(math.floor(data["avg"]*1000)/10)




        # 輔助電源耗電量(kwh)
        ID=db.equipment_status.find_one({"name":"輔助電源電表"})["ID"]
        match = {"ID": ID, "time": {"$gte": start, "$lt": end}}
        project = {
                        "day": {"$dayOfMonth": "$time"},
                        "month": {"$month": "$time"},
                        "tot_kwh": 1
                    }
        group = {"_id": "$day",
                "lastValue": {"$last": "$tot_kwh"},
                "firstValue": {"$first": "$tot_kwh"}}
        project2 = {"_id": "$_id",
            "tot_kwh": {"$subtract": ["$lastValue","$firstValue"]}
        }
        if mode == "month":
            group["_id"]="$day"
        else:
            group["_id"]="$month"
        #### every day or month

        dataList = list(db.acm.aggregate(   [
                    {'$match': match},
                    {"$project":project},  
                    {'$group': group},
                    {"$project":project2},  

                ]))    
        print(dataList)
        apm_data={}
        for data in dataList:
            try:
                # print(data)
                table.find(id="dataRow{}".format(data["_id"])).find_all(class_="dataCol")[
                    2].string = str(round(data["tot_kwh"], 1))
                apm_data[data["_id"]]=data["tot_kwh"]

            except Exception as e :  
                traceback.print_exc()
                pass
        
        #### AVG
        group["_id"]=None
        dataList = list(db.acm.aggregate(   [
                        {'$match': match},
                        {"$project":project},  
                        {'$group': group},
                        {"$project":project2},  

                    ]))
        # print(dataList)
        
        for data in dataList:
            try:
                table.find(id="a_tot_kwh").string = str(round(data["tot_kwh"], 1))
                sum_apm_data=data["tot_kwh"]
            except Exception as e :  
                traceback.print_exc()
                pass




        # 高壓側輸出電能
        # 高壓側輸入電能
        # 高壓側損失電能
        ID=db.equipment_status.find_one({"name":"高壓側電表"})["ID"]

        match = {"ID": ID, "time": {"$gte": start, "$lt": end}}
        project = {
                        "day": {"$dayOfMonth": "$time"},
                        "month": {"$month": "$time"},
                        "exp_kwh": 1,
                        "imp_kwh": 1,
                        "net_kwh": 1
                    }
        group = {"_id": "$day",
                                "last_exp_kwh": {"$last": "$exp_kwh"},
                                "first_exp_kwh": {"$first": "$exp_kwh"},
                                "last_imp_kwh": {"$last": "$imp_kwh"},
                                "first_imp_kwh": {"$first": "$imp_kwh"},
                                "last_net_kwh": {"$last": "$net_kwh"},
                                "first_net_kwh": {"$first": "$net_kwh"}}
        project2 = {"_id": "$_id",
            "exp_kwh": {"$subtract": ["$last_exp_kwh","$first_exp_kwh"]},
            "imp_kwh": {"$subtract": ["$last_imp_kwh","$first_imp_kwh"]},
            "net_kwh": {"$subtract": ["$last_net_kwh","$first_net_kwh"]},


        }
        if mode == "month":
            group["_id"]="$day"
        else:
            group["_id"]="$month"

        #### every day or month
        dataList = list(db.acm.aggregate(   [
                    {'$match': match},
                    {"$project":project},  
                    {'$group': group},
                    {"$project":project2},  

                ]))
        # print(dataList)
        hpm_data={}

        for data in dataList:
            # print("dataRow{}".format(data["_id"]))
            # print(table.find(id="dataRow{}".format(  data["_id"])).find_all(class_="dataCol"))

            try:


                hpm_data[data["_id"]]=data["imp_kwh"]+data["exp_kwh"]

                table.find(id="dataRow{}".format(data["_id"])).find_all(class_="dataCol")[
                        4].string = str(round(data["imp_kwh"], 1))
                table.find(id="dataRow{}".format(data["_id"])).find_all(class_="dataCol")[
                        5].string = str(round(data["exp_kwh"], 1))
                table.find(id="dataRow{}".format(data["_id"])).find_all(class_="dataCol")[
                        6].string = str(round(data["net_kwh"], 1))
                table.find(id="dataRow{}".format(data["_id"])).find_all(class_="dataCol")[
                       7].string = str(round(   (data["imp_kwh"] /data["exp_kwh"])*100    , 1))

                        
            except Exception as e :  
                traceback.print_exc()
                pass
        #### AVG
        group["_id"]=None
        dataList = list(db.acm.aggregate(   [
                    {'$match': match},
                    {"$project":project},  
                    {'$group': group},
                    {"$project":project2},  

                ]))   
        # print(dataList)
        
        for data in dataList:
            try:
                sum_hpm_data=data["imp_kwh"]+data["exp_kwh"]
                table.find(id="h_exp_kwh").string = str(
                    round(data["imp_kwh"], 1))
                table.find(id="h_imp_kwh").string = str(
                    round(data["exp_kwh"], 1))
                table.find(id="h_net_kwh").string = str(
                    round(data["net_kwh"], 1))
                table.find(id="battery_loss").string = str(
                    round((data["imp_kwh"]/data["exp_kwh"])*100, 1))
       
            except Exception as e :  
                        traceback.print_exc()
                        pass

        #輔助電源損耗率
        for key in apm_data.keys():
            try:
                lossRate=abs((apm_data[key]-hpm_data[key])*100/hpm_data[key])
                table.find(id="dataRow{}".format(key)).find_all(class_="dataCol")[
                        3].string = str(round(lossRate, 1))

            except Exception as e :  
                        traceback.print_exc()
                        pass

        try:
            table.find(id="apm_loss").string = str(
                round(abs((sum_apm_data-sum_hpm_data)*100/sum_hpm_data), 1))
        except:
            table.find(id="apm_loss").string = ""



        mbmsData=db.mbms.find_one({"time": {"$gte": start, "$lt": end}},{"time":1,"soh_sys":1}, sort=[('time', -1)])
        # print(mbmsData)
        if mbmsData :
            table.find(id="soh_sys".format(data["_id"])).string = str(round(mbmsData["soh_sys"], 1))

        output = soup.encode(formatter="html5")
        output = output.decode("utf-8")
        # print(output)

        htmldoc = HTML(string=output, base_url="")
        pdf=htmldoc.write_pdf(stylesheets=[os.path.join(dirpath,"./static/css/reportTemplate.css")])

        db.report.insert({"time":end,"FileName":title,"type":mode,"file":pdf})

        return pdf
    else:
        print("status不在本機執行！！")

def run():
    date = datetime.datetime(
        year=2021, month=10, day=1, hour=1, minute=0, second=0, microsecond=0)
    # pdf = genReport(date,"month")
    # pdf = genReport("quarter",date)
    # pdf = genReport("quarter",date)
    pdf = genReport("month",date)

    # date = datetime.datetime(
    #     year=2022, month=1, day=1, hour=0, minute=0, second=0, microsecond=0)
    # pdf = genReport("year",date)

    if pdf:
        Path("./report.pdf").write_bytes(pdf)
        pass

if __name__ == "__main__":
    run()
