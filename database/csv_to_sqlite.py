import csv
import sqlite3
import pandas as pd

def createTableHBSample(cursor):
    createTable = '''CREATE TABLE IF NOT EXISTS HBSample(
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    numHeartbeats INTEGER NOT NULL,
                    avgDuration REAL NOT NULL,
                    hbKey TEXT NOT NULL,
                    sampleKey INTEGER NOT NULL);
                    '''
    cursor.execute(createTable)

def insertIntoHBSample(df, cursor):
    tableName = "HBSample"
    columnCount = len(df.columns)

    for i in range(len(df)):
        for j in range(0, columnCount, 2):
            numberHeartbeatrs = df.iloc[i, j]
            avgDuration = df.iloc[i, j+1]
            columnName = df.columns[j]
            sql = "INSERT INTO " \
                    + tableName \
                    + " (numHeartbeats,avgDuration,hbKey,sampleKey) VALUES " \
                    + "(" \
                    + str(numberHeartbeatrs) \
                    + "," \
                    + str(avgDuration) \
                    + ",'" \
                    + columnName \
                    + "'," \
                    + str(0) \
                    + ")"
            #insert into table one row at a time
            cursor.execute(sql)

def printDataFromHBSample():
    connection = sqlite3.connect('appekg.db')
    cursor = connection.cursor()

    selectAll = "SELECT * FROM HBSample"
    rows = cursor.execute(selectAll).fetchall()
     
    # Output to the console screen
    for r in rows:
        print(r)

    cursor.close()
    connection.commit()
    connection.close()

# read from csv
filePath = "appekg-16860.csv"
df = pd.read_csv(filePath)
# temporary: drop irrelevant columns for HBSample table
df = df.drop(df.columns[[0, 1]],axis = 1)

# create and/or connect to db
connection = sqlite3.connect('appekg.db')
cursor = connection.cursor()

# create HBSample table if already not exists
createTableHBSample(cursor)

# insert data into HBSample table from csv file using dataframe
insertIntoHBSample(df, cursor)

# close cursor and connection
cursor.close()
connection.commit()
connection.close()

# show data from HBSample table
printDataFromHBSample()


 


