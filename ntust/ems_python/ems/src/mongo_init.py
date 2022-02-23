def createIndex():
    colList = ["sys_control","site_control","iec61850","SPM","ASPM","TPC","alarm","SOE","pc_info","sys_protect_log",
               "pcs","pcs_control","pcs_set","mbms","mbms_control","acm","ups","env","rio","access","air_control"]
    for i in colList:
        col = db[i]
        col.create_index([("ID", 1),("time", 1)])
        col.create_index([("ID", 1),("time", -1)])
        col.create_index([("time", 1)])
        col.create_index([("time", -1)])

def emptyCol():
    db.equipment.remove({})
    db.model.remove({})
    db.program.remove({})
    db.equipment_status.remove({})


def insertEquipment():
    with open('equipment.json',encoding="utf-8") as json_file:
        # print(json_file)
        data = json.load(json_file)
        for i in data:
            if "_id" in i:
                i["_id"] = ObjectId(i["_id"])
            try:
                db.equipment.insert(i)
            except:
                pass

