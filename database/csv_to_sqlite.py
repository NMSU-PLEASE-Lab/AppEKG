import csv
import sqlite3
import pandas as pd
import json

def insert(cursor, tableName, columnNames, values):
    sql = "INSERT INTO " + tableName + " " + columnNames + " VALUES " + "(" + values + ");"
    cursor.execute(sql)

def insertIntoProcess(cursor, tableName, columnNames, data):  
    values = str(data["pid"]) + "," + str(0) + "," + str(data["jobid"]) + "," + str(data["rank"]) + "," + str(0)
    insert(cursor, tableName, columnNames, values)

def insertIntoEKGSample(cursor, tableName, columnNames, df):
    for index in df.index:
        values = str(df.loc[index]['timemsec']) + "," + str(df.loc[index]['threadID']) + "," + str(0)
        insert(cursor, tableName, columnNames, values)
    
def insertIntoHeartbeat(cursor, tableName, columnNames, data):
    for hbId, hbName in data["hbnames"].items():
        values = str(data["pid"]) + "," + str(data["jobid"]) + "," + str(data["appid"])
        insert(cursor, tableName, columnNames, values)

def insertIntoHBSample(cursor, tableName, columnNames, df):
    hbIdCount = len(df.columns)
    df = df.drop(df.columns[[0, 1]], axis = 1)
    
    for i in range(len(df)):
        for j in range(0, hbIdCount, 2):
            numberHeartbeatrs = df.iloc[i, j]
            avgDuration = df.iloc[i, j+1]
            hbKey = df.columns[j]
            values = str(numberHeartbeatrs) + "," + str(avgDuration) + "," + hbKey + "," + str(0)
            insert(cursor, tableName, columnNames, values)

def printData(tableName):
    connection = sqlite3.connect('appekg.db')
    cursor = connection.cursor()

    selectAll = "SELECT * FROM " + tableName
    rows = cursor.execute(selectAll).fetchall()
     
    # Output to the console screen
    for r in rows:
        print(r)

    cursor.close()
    connection.commit()
    connection.close()

# read from csv and json
csvFilePath = "appekg-67624.csv"
jsonFilePath = "appekg-67624.json"
dfCSV = pd.read_csv(csvFilePath)
# dfJSON = pd.read_json(jsonFilePath)
dataJSON = json.load(open(jsonFilePath, 'r'))

# create and/or connect to db
connection = sqlite3.connect('appekg.db')
cursor = connection.cursor()

# tableName = "Process"
# columnNames = "time, threadId, processKey"
# insertIntoProcess(cursor, tableName, columnNames, dataJSON)

# tableName = "EKGSample"
# columnNames = "time, threadId, processKey"
# insertIntoEKGSample(cursor, tableName, columnNames, dfCSV)

# tableName = "Heartbeat"
# columnNames = "time, threadId, processKey"
# insertIntoHeartbeat(cursor, tableName, columnNames, dataJSON)

# tableName = "HBSample"
# columnNames = "numHeartbeats, avgDuration, hbKey, sampleKey"
# insertIntoHBSample(cursor, tableName, columnNames, dfCSV)

# close cursor and connection
cursor.close()
connection.commit()
connection.close()

# show data from table
# printData("HBSample")
