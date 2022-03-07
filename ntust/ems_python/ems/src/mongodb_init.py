import pandas as pd
import pymongo
import json
from bson.objectid import ObjectId
import datetime

#
# conn = pymongo.MongoClient(['192.168.1.2:27017','192.168.1.3:27017'],replicaset='rs0',serverSelectionTimeoutMS=10000)
# conn = pymongo.MongoClient(['192.168.1.3:27017'],replicaset='rs0',serverSelectionTimeoutMS=10000)
# conn = pymongo.MongoClient('mongodb://root:pc152@127.0.0.1:27017/') 
conn = pymongo.MongoClient('mongodb://user:pwd@ems1:27017,ems2:27017', replicaset='rs0',
                           serverSelectionTimeoutMS=10)
# conn["admin"].authenticate("root","pc152")
db = conn['AFC']


def create_index():
    col_list = ["sys_control", "site_control", "iec61850", "SPM", "ASPM", "TPC", "alarm", "SOE", "pc_info",
               "sys_protect_log", "pcs",
               "pcs_control", "pcs_set", "mbms", "mbms_control", "acm", "ups", "env", "rio", "access", "air_control"]
    for i in col_list:
        col = db[i]
        col.create_index([("ID", 1), ("time", 1)])
        col.create_index([("ID", 1), ("time", -1)])
        col.create_index([("time", 1)])
        col.create_index([("time", -1)])


def empty_col():
    db.equipment.delete_many({})
    db.model.delete_many({})
    db.program.delete_many({})
    db.equipment_status.delete_many({})


def insert_equipment():
    with open('../resource/equipment.json', encoding="utf-8") as json_file:
        # print(json_file)
        data = json.load(json_file)
        for i in data:
            if "_id" in i:
                i["_id"] = ObjectId(i["_id"])
            try:
                db.equipment.insert(i)
            except:
                pass


def insert_model():
    # with open('../resource/model.json', encoding="utf-8") as json_file:
    #     data = json.load(json_file)
    #     db.model.insert(data)

    df = pd.read_excel(
        "../resource/io_list.xlsx",
        sheet_name="MBMS資料表",
        usecols=["Index", "tag", "name", "tagType", "error", "unit", "round", "plotType", "y_title", "bitIndex",
                 "bitName", "translate", "color", "alarm", "EMS"]
    )
    df = df.where(pd.notnull(df), None)
    tags = generate_tags(df)
    insert_model_to_db("P3A", "mbms", "NA", tags)

    df = pd.read_excel(
        "../resource/io_list.xlsx",
        sheet_name="UPS資料表",
        usecols=["Index", "tag", "name", "tagType", "error", "unit", "round", "plotType", "y_title", "bitIndex",
                 "bitName", "translate", "color", "alarm", "EMS"]
    )
    df = df.where(pd.notnull(df), None)
    tags = generate_tags(df)
    insert_model_to_db("ups", "ups", "NA", tags)


def insert_program():
    with open('../resource/program.json', encoding="utf-8") as json_file:
        # print(json_file)
        data = json.load(json_file)
        db.program.insert_many(data)


def insert_status():
    with open('../resource/equipment_status.json', encoding="utf-8") as json_file:
        # print(json_file)
        data = json.load(json_file)
        db.equipment_status.insert_many(data)


def number_data(row):
    try:
        number = {"unit": row["unit"], "round": int(row["round"]), "plotType": row["plotType"],
                  "y_title": row["y_title"]}
    except:
        print(row)
    try:
        for i in row["alarm"].split("&&"):
            number["alarm"].append(json.loads(i))
    except:
        pass
    return number


def bit_data(row):
    bit = {"index": int(row["bitIndex"]), "name": row["bitName"]}
    try:
        bit["translate"] = json.loads("{" + row["translate"] + "}")
    except:
        bit["translate"] = {}
    try:
        bit["color"] = json.loads("{" + row["color"] + "}")
    except:
        bit["color"] = {}
    bit["alarm"] = []
    try:
        for i in row["alarm"].split("&&"):
            bit["alarm"].append(json.loads(i))
    except:
        pass
    return bit


def text_data(row):
    try:
        row["translate"] = json.loads("{" + row["translate"] + "}")
    except:
        row["translate"] = {}
    text = {"translate": row["translate"], "color": row["color"], "alarm": []}
    try:
        for i in row["alarm"].split("&&"):
            text["alarm"].append(json.loads(i))
    except:
        pass
    return text


def generate_tags(df):
    tags = []
    for num, row in df.iterrows():
        sameIndex = next((index for index, tag in enumerate(tags) if tag['tag'] == row["tag"]), None)
        if row["EMS"] != None:
            if sameIndex == None:
                oneline = {"tag": row["tag"], "name": row["name"], "tagType": row["tagType"], "error": row["error"]}
                if row["tagType"] == "number" or row["tagType"] == "numberArray" or row["tagType"] == "numberArray2" or \
                        row["tagType"] == "numberArray3":
                    oneline = {**oneline, **number_data(row)}
                elif row["tagType"] == "bits" or row["tagType"] == "bitsArray" or row["tagType"] == "bitsArray2":
                    oneline = {**oneline, "bits": [bit_data(row)]}
                elif row["tagType"] == "text" or row["tagType"] == "textArray" or row["tagType"] == "textArray2":
                    oneline = {**oneline, **text_data(row)}
                tags.append(oneline)
            else:
                if row["tagType"] == "bits" or row["tagType"] == "bitsArray" or row["tagType"] == "bitsArray2":
                    tags[sameIndex]["bits"].append(bit_data(row))
                else:
                    pass
    return tags


def insert_model_to_db(ID, equipType, error, tags):
    # print(ID,equipType,error,tags)
    db.model.insert_one(
        {
            "ID": ID,
            "type": equipType,
            "error": error,
            "tags": tags
        }
    )


def init_parameter():
    now = datetime.datetime.now()
    db.sys_control.insert_one({"ID": "system", "time": now, "protection": 0})
    scale_set = {}
    for i in db.equipment.find({"type": "pcs"}):
        scale_set[str(i["_id"])] = 0
    db.site_control.insert_one(
        {"ID": "site_control", "time": now, "soc_max": 0, "soc_min": 0, "System_p_max": 0, "System_p_min": 0,
         "System_q_max": 0, "System_q_min": 0, "mode": 0, "f1_line_set": 59.5, "f2_line_set": 59.75,
         "f3_line_set": 59.98, "f4_line_set": 60.02, "f5_line_set": 60.25, "f6_line_set": 60.5, "p1_line_set": 100,
         "p2_line_set": 48, "p3_line_set": 9, "p4_line_set": -9, "p5_line_set": -48, "p6_line_set": -100,
         "FP_line_p_base": 0, "FP_type": 0, "Vq_v1_set": 0, "Vq_v2_set": 0, "Vq_v3_set": 0, "Vq_v4_set": 0,
         "Vq_v5_set": 0, "Vq_v6_set": 0, "Vq_v7_set": 0, "Vq_q1_set": 0, "Vq_q2_set": 0, "Vq_q3_set": 0, "Vq_q4_set": 0,
         "Vq_q5_set": 0, "Vq_q6_set": 0, "Vq_q7_set": 0, "Vq_q_base": 0, "Vq_v_base": 0, "scale": scale_set}
    )


if __name__ == '__main__':
    create_index()
    empty_col()
    insert_equipment()
    insert_model()
    insert_program()
    insert_status()
    init_parameter()
