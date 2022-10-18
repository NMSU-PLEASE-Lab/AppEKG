#
# TODO: add proper docs
#
# 
#

import pandas as pd
import sys
from os import listdir
import numpy as np
import csv
import matplotlib.pyplot as plt
import os
import json

# Find files by extension
def list_files(dir_path, fileExt):
    files = listdir(dir_path)
    return [csv for csv in files if csv.endswith(fileExt)]

# sort csv file in assending orders
def sortFiles(files):
    return sorted((files))

# extract the required data from the csv file and create a dataframe
def extractDataFromCSV(dataFile):    
    file2 = open(dataFile,"r")
    df = pd.read_csv(file2)
    return df

# extracte the unique thread IDs from the dataframe
def extractTHreadIDS(df):
    threadIDs = df.threadID.unique()
    return threadIDs

# convert a json file to a dictionary
def readJsonFile(jsonFile):
    data = json.load(jsonfile)
    return data

# get number of HBs and HB names form a dataframe
def getNoHBs(df):
    noOfHBs = 0
    hbNames = {}
    for col in df.columns:
        if "hbcount" in col:
            noOfHBs += 1
            hbNames[noOfHBs] = "hbeat" + str(noOfHBs)
    return noOfHBs, hbNames


# get # HBs and HB names
def createHBNames(data, df):
    hbNames = {}
    noOfHBs = 0
    # get # HBs and HB names from json file if not empty, and from csv file otherwise
    if data["hbnames"]:
        for hb in data["hbnames"].values():
            noOfHBs += 1
            hbNames[noOfHBs] = hb
    else:
        noOfHBs, hbNames = getNoHBs(df)
    
    return(noOfHBs, hbNames)

# plot HB duration
def plotHBTime(df, appname, numofHBs, hbNames, rank, threadID):
    i = 1
    fig=plt.figure(figsize=(7.2,4.45))
    for i in range(1,numofHBs+1):
        plt.plot(df["timemsec"], df["hbduration" + str(i)],colors[i-1],label = hbNames[i])
        # for i,func in enumerate(hbNames):
        #     plt.plot(df[headers2[i]],colors[i],label = func)
        # Set the y-axis scale log base 10
        #plt.ylim(bottom=0.1) # not sure if we need this on time plots
    plt.yscale("log",base = 10)
    plt.xlabel("Time (sec)",fontsize=12)
    plt.ylabel("Average Time (milisec)",fontsize=12)
    # position the legend
    plt.legend(loc='best',prop={'size':12})
    # x and y ticks font size
    plt.xticks(fontsize=12)
    plt.yticks(fontsize=12)
    # plt.legend(bbox_to_anchor=(0.9, 0.69))
    # Save the figure as png
    plt.savefig('HBTime{}Rank{}threadID{}.png'.format(appname, rank, threadID), bbox_inches="tight", dpi=100)
    plt.show()

# plot HB count
def plotHBCount(df, appname, numofHBs, hbNames, rank, threadID):
    fig=plt.figure(figsize=(7.2, 4.45))
    for i in range(1,numofHBs + 1):
        plt.plot(df["timemsec"], df["hbcount" + str(i)],colors[i-1], label = hbNames[i])
        # for i,func in enumerate(hbNames):
        #     plt.plot(df[headers2[i]],colors[i],label = func)
        # Set the y-axis scale log base 10
    plt.ylim(bottom=0.1)
    plt.yscale("log", base = 10)
    plt.xlabel("Time (msec)", fontsize=12)
    plt.ylabel("Interval Heartbeat Count", fontsize=12)
    # position the legend
    # plt.legend(bbox_to_anchor=(0.55, 0.7))
    # position the legend
    plt.legend(loc='best',prop={'size':12})
    # x and y ticks font size
    plt.xticks(fontsize=12)
    plt.yticks(fontsize=12)
    # Save the figure as png
    plt.savefig('HBCount{}Rank{}threadID{}.png'.format(appname, rank, threadID),bbox_inches="tight",dpi=100)
    plt.show()

# plot hbcount and hbtime for all ranks(each rank has 2 figures, one hbcount and one for hbtime)
def plotRanksData(path, files, appname, numofHBs, funcNames):
    # TODO: either all ranks or a specific rank
    for i, _ in enumerate(files):
        df = extractDataFromCSV(path + files[i])
        print(getNoHBs(df))
        threadIDs = extractTHreadIDS(df)
        for j in threadIDs:
            plotHBCount(df.loc[df['threadID'] == j], appname, numofHBs, funcNames, i, j)
            plotHBTime(df.loc[df['threadID'] == j], appname, numofHBs,funcNames, i, j)


##### Main #####
if len(sys.argv) != 3:
    print("Usage: {0} <dataDir>  <appName>".format(sys.argv[0]))
    exit(1)

dirPath = sys.argv[1]
appname = sys.argv[2]

# find csv files 
csvFiles = list_files(dirPath, ".csv")
jsonFiles = list_files(dirPath, ".json")
jsonfile = open(jsonFiles[0],"r")
data = json.load(jsonfile)
# sort csv files in assending order
sortedFiles = sortFiles(csvFiles)
df = extractDataFromCSV(dirPath + sortedFiles[0])
numofHBs,funcNames = createHBNames(data,df)
colors = ['r', 'g', 'b', 'y', 'k','c','m']
# plot the data of each rank
plotRanksData(dirPath, sortedFiles, appname, numofHBs, funcNames)