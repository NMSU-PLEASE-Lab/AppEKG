"""

This script reads csv and json files from a folder and insert the data into a sqlite database.

To run the script:

$ python csv-to-sqlite.py input_path output_path

input_path = the folder where the csv and json files are located
output_path = the database location -->> not used; for future use

"""

import csv
import sqlite3
import pandas as pd
import json
import glob
import sys
import os

def insert(tableName, columnNames, values):
    # create and/or connect to db
    connection = sqlite3.connect('appekg.db')
    cursor = connection.cursor()

    # execute the insert query
    for value in values:
        sql = "INSERT INTO " + tableName + " (" + columnNames + ") VALUES " + "(" + value + ");"
        cursor.execute(sql)

    # commit and close
    cursor.close()
    connection.commit()
    connection.close()

def insertIntoApplication(tableName, columnNames, data):
    values = []
    id = data[0]
    name = data[1]
    version = data[2]
    values.append(str(id) + "," + "'" + name + "'" + "," + "'" + version + "'")
    # print("insertIntoApplication: ")
    # print(values)
    insert(tableName, columnNames, values)

def insertIntoJob(tableName, columnNames, data):
    values = []
    jobId = data['jobid']
    user = 0 # hardcoded 0 value; need to replace
    beginTime = data['starttime']
    endTime = 0 # hardcoded 0 value; need to replace
    numNodes = 0 # hardcoded 0 value; need to replace
    numProcesses = 0 # hardcoded 0 value; need to replace
    appId = data['appid']
    values.append(str(jobId) + "," + str(user) + "," + str(beginTime) + "," + str(endTime) + "," + str(numNodes) + "," + str(numProcesses) + "," + str(appId))
    # print("insertIntoJob: ")
    # print(values)
    insert(tableName, columnNames, values)

def insertIntoProcess(tableName, columnNames, data):
    values = []
    pid = data['pid']
    nodeId = 0 # hardcoded 0 value; need to replace
    jobId = data['jobid']
    rank = data['rank']
    numThreads = 0 # hardcoded 0 value; need to replace

    values.append(str(pid) + "," + str(nodeId) + "," + str(jobId) + "," + str(rank) + "," + str(numThreads))
    # print("insertIntoProcess: ")
    # print(values)
    insert(tableName, columnNames, values)

def insertIntoEKGSample(tableName, columnNames, df, data):
    values = []
    pid = data['pid']
    nodeId = 0 # hardcoded 0 value; need to replace
    jobId = data['jobid']

    processId = getProcessId(pid, nodeId, jobId)

    for index in df.index:
        time = int(df.loc[index]['timemsec'])
        threadId = int(df.loc[index]['threadID'])
        values.append(str(time) + "," + str(threadId) + "," + str(processId))
    
    # print("insertIntoEKGSample: ")
    # print(values)
    insert(tableName, columnNames, values)
    
def insertIntoHeartbeat(tableName, columnNames, data):
    values = []
    for hbId, hbName in data['hbnames'].items():
        values.append(str(hbId) + "," + "'" + str(hbName) + "'" + "," + str(data['appid']))
    
    # print("insertIntoHeartbeat: ")
    # print(values)
    insert(tableName, columnNames, values)

def insertIntoHBSample(tableName, columnNames, df, data):
    values = []
    #df = df.drop(df.columns[[0, 1]], axis = 1)
    columnNumber = len(df.columns)

    pid = data['pid']
    nodeId = 0 # hardcoded 0 value; need to replace
    jobId = data['jobid']

    processId = getProcessId(pid, nodeId, jobId)

    hbIds = []
    for hbId, hbName in data['hbnames'].items():
        hbIds.append(hbId)

    for i in range(0, len(df)):
        time = int(df.loc[i]['timemsec'])
        sampleId = getSampleId(time, processId)

        # todo: need to fix the complicated calculation for the j value increment
        for j in range(0, int((columnNumber-2)/2)):
            numberHeartbeats = df.iloc[i, (j*2)+2]
            avgDuration = df.iloc[i, ((j*2)+1)+2]
            hbId = hbIds[j]
            values.append(str(numberHeartbeats) + "," + str(avgDuration) + "," + str(hbId) + "," + str(sampleId))
    
    # print("insertIntoHBSample: ")
    # print(values)
    insert(tableName, columnNames, values)

def getAllProcessId(data):
    values = []
    pid = data['pid']
    nodeId = 0 # hardcoded 0 value; need to replace
    jobId = data['jobid']

    processIds = getProcessId(pid, nodeId, jobId)
    print("all process ids: ")
    print(processIds)

def getSampleId(time, processId):
    connection = sqlite3.connect('appekg.db')
    cursor = connection.cursor()

    sql = "SELECT id from EKGSample WHERE \
                time={time} AND \
                process_id={processId}".format(time = time, processId = processId)

    sampleId = cursor.execute(sql).fetchone()

    cursor.close()
    connection.commit()
    connection.close()

    return sampleId[0]

def getProcessId(pid, nodeId, jobId):
    connection = sqlite3.connect('appekg.db')
    cursor = connection.cursor()

    sql = "SELECT id from Process WHERE \
                pid={pid} AND \
                node_id={nodeId} AND \
                job_id={jobId}".format(pid = pid, nodeId = nodeId, jobId = jobId)

    processId = cursor.execute(sql).fetchone()

    cursor.close()
    connection.commit()
    connection.close()

    return processId[0]

def printData(tableName):
    connection = sqlite3.connect('appekg.db')
    cursor = connection.cursor()

    selectAll = "SELECT * FROM " + tableName
    rows = cursor.execute(selectAll).fetchall()
     
    # Output to the console screen
    print("--- " + tableName + " ---")
    print()

    for r in rows:
        print(r)

    print()

    cursor.close()
    connection.commit()
    connection.close()

inputPath = sys.argv[1]
outputPath = sys.argv[2]

filenamesCSV = [f for f in os.listdir(inputPath) if f.endswith('.csv')]
filenamesJson = [f for f in os.listdir(inputPath) if f.endswith('.json')]

# sort the files by name
filenamesCSV.sort()
filenamesJson.sort()

fileCount = len(filenamesCSV) # or, len(filenamesJson)

# create and/or connect to db
connection = sqlite3.connect('appekg.db')
cursor = connection.cursor()

jsonFilePath = inputPath + "/" + filenamesJson[0]
jsonFile = open(jsonFilePath, 'r')
dataJSON = json.load(jsonFile)

# Application table - static data
tableName = "Application"
columnNames = "id, name, version"
data = (dataJSON['appid'], 'APP_NAME', 'v2.0')
insertIntoApplication(tableName, columnNames, data)

# Job table - possible to fill in with any of the json files
tableName = "Job"
columnNames = "id, user, begin_time, end_time, num_nodes, num_processes, app_id"
insertIntoJob(tableName, columnNames, dataJSON)

# Heartbeat table - possible to fill in with any of the json files
tableName = "Heartbeat"
columnNames = "id, name, app_id"
insertIntoHeartbeat(tableName, columnNames, dataJSON)

# close cursor and connection
cursor.close()
connection.commit()
connection.close()

for i in range(0, fileCount):
    # read from csv and json
    csvFilePath = inputPath + "/" + filenamesCSV[i]
    jsonFilePath = inputPath + "/" + filenamesJson[i]

    dfCSV = pd.read_csv(csvFilePath)
    jsonFile = open(jsonFilePath, 'r')
    dataJSON = json.load(jsonFile)
    jsonFile.close

    tableName = "Process"
    columnNames = "pid, node_id, job_id, rank, num_threads"
    insertIntoProcess(tableName, columnNames, dataJSON)

    tableName = "EKGSample"
    columnNames = "time, thread_id, process_id"
    insertIntoEKGSample(tableName, columnNames, dfCSV, dataJSON)

    tableName = "HBSample"
    columnNames = "num_heartbeats, avg_duration, hb_id, sample_id"
    insertIntoHBSample(tableName, columnNames, dfCSV, dataJSON)

# show data from table
printData("Application")
printData("Job")
printData("Process")
printData("Heartbeat")
printData("EKGSample")
printData("HBSample")

