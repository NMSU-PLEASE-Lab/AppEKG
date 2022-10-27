
#
# This script plots heartbaet counts and durations. It reads the CSV and JSON files that were
# produced by the AppEKG tool and then plots the heartbeat data. It plots heartbeat data per
# threadID, per processID and per threadID per processID. Each created plot contains a plot of 
# each heartbeat.
# Requirements:
#   pandas>=1.5.0
#   numpy>=1.23.3
#   matplotlib>=3.6.0
#   argparse>=1.4.0
# 
# How to install requirements?
#   pip3 install -r requirements.txt
# 
# How to run the script?
#   python3 plotting.py [options]
# 
# Options:
#   --input or -i: required to specify heartbeat data path. Make sure the heartbeat data in the
#     the directory is from a single run.
#   --output or -o: optional to specify where to save the plots. If not defined, the input path
#     is used.
#   --type or -t: optional to select what to plot. If not defined all plot types is produced 
#     (per threadID, per processID, per threadID per processID).
#     Per-pid: plot heartbeat data per-processID only. If the app were run on 4 processes, 8 plots
#     are created. (4 heartbeat counts and 4 heartbeat durations).
#     Per-tid: plot heartbeat data per-threadID only. If the app were run on 4 threads, 8 plots are 
#     created (4 heartbeat counts and 4 heartbeat durations).
#     Per-tid-pid: plot heartbeat data per-threadID per-processID. If the app were run on 4 threads 
#     and 2 processes, 16 plots are created (8 heartbeat counts and 8 heartbeat durations), where each
#     plot represents the data per-threadID per-processID.
# Examples:
#   1. python3 plotting.py --input "/path/toMyInput" --type per-pid
#     To plot hearbeat data located in "/path/toMyInput" per processID and save plots in 
#     "/path/toMyInput"
#   2. python -i /path/toHBData/ -o /path/where/toSavePlots -t per-tid
#     To plot heartbeat data located in /path/toHBData/ per threadID and save the plots in
#     "/path/where/toSavePlots"
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
import argparse 

pltType = 'all'
pid = []
threadIDs = []
ranks = {}


#---------------------------------------------------------------------
# find files by extension
#---------------------------------------------------------------------
def list_files(dir_path, fileExt):
    if not(os.path.exists(dir_path)):
        print("Input dir {} not exist".format(dir_path))
        exit(1)
    files = listdir(dir_path)
    return [csv for csv in files if csv.endswith(fileExt)]

#---------------------------------------------------------------------
# convert csv to a dataframe
#---------------------------------------------------------------------
def extractDataFromCSV(dataFile):    
    file2 = open(dataFile,"r")
    df = pd.read_csv(file2)
    return df

#---------------------------------------------------------------------
# extracte the unique thread IDs from the dataframe
#---------------------------------------------------------------------
def extractTHreadIDS(df):
    threadIDs = df.threadID.unique()
    return threadIDs

#---------------------------------------------------------------------
# convert a json file to a dictionary
#---------------------------------------------------------------------
def readJsonFile(jsonFile):
    data = json.load(jsonfile)
    return data

#---------------------------------------------------------------------
# get number of HBs and HB names form a dataframe
#---------------------------------------------------------------------
def getNoHBs(df):
    noOfHBs = 0
    hbNames = {}
    for col in df.columns:
        if "hbcount" in col:
            noOfHBs += 1
            hbNames[noOfHBs] = "hbeat" + str(noOfHBs)
    return noOfHBs, hbNames

#---------------------------------------------------------------------
# get # HBs and HB names
#---------------------------------------------------------------------
def createHBNames(path, files, df):
    hbNames = {}
    noOfHBs = 0
    processIDs = []
    ranks = {}
    # get # HBs and HB names from json file if not empty, and from csv file otherwise
    for file in files:
        data = json.load(open(path + file, 'r'))
        ranks[data["pid"]] = data["rank"]
        # processIDs.append(data["pid"])
        # ranks.append(data["rank"])
    if data["hbnames"]:
        for hbId in data["hbnames"].keys():
            noOfHBs += 1
            hbNames[hbId] = data["hbnames"][hbId]
    else:
        noOfHBs, hbNames = getNoHBs(df)
    
    return(ranks, noOfHBs, hbNames)

#---------------------------------------------------------------------
# plot HB duration 
#---------------------------------------------------------------------
def plotHBDuration(df, numofHBs, hbNames, rank, threadID, outPath, plotType):
    i = 1
    fig=plt.figure(figsize=(7.2,4.45))
    for i in range(1,numofHBs+1):
        plt.plot(df["timemsec"], df["hbduration" + str(i)],colors[i-1],label = hbNames[str(i)])
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
    if plotType == "per-pid":
        plt.title(label="Heartbeat Durations Rank ({})".format(rank))
        plt.savefig(outPath + 'HBDuration-Rank{}.png'.format(rank), bbox_inches="tight", dpi=100)
    elif plotType == "per-tid-pid":
        plt.title(label="Heartbeat Durations Rank ({}), ThreadID ({})".format(rank, threadID))
        plt.savefig(outPath + 'HBDuration-Rank{}-ThreadID{}.png'.format(rank, threadID), bbox_inches="tight", dpi=100)
    elif plotType == "per-tid":
        plt.title(label="Heartbeat Durations ThreadID ({})".format(threadID))
        plt.savefig(outPath + 'HBDuration-ThreadID{}.png'.format(threadID), bbox_inches="tight", dpi=100)

    # plt.show()

#---------------------------------------------------------------------
# plot HB count 
#---------------------------------------------------------------------
def plotHBCount(df, numofHBs, hbNames, rank, threadID, outPath, plotType):
    fig=plt.figure(figsize=(7.2, 4.45))
    for i in range(1,numofHBs + 1):
        plt.plot(df["timemsec"], df["hbcount" + str(i)],colors[i-1], label = hbNames[str(i)])
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
    if plotType == "per-pid":
        plt.title(label="Heartbeat Count Rank ({})".format(rank))
        plt.savefig(outPath + 'HBCount-Rank{}.png'.format(rank), bbox_inches="tight", dpi=100)
    elif plotType == "per-tid-pid":
        plt.title(label="Heartbeat Count Rank ({}), ThreadID ({})".format(rank, threadID))
        plt.savefig(outPath + 'HBCount-Rank{}-threadID{}.png'.format(rank, threadID),bbox_inches="tight",dpi=100)
    elif plotType == "per-tid":
        plt.title(label="Heartbeat Count ThreadID ({})".format(threadID))
        plt.savefig(outPath + 'HBCount-threadID{}.png'.format(threadID),bbox_inches="tight",dpi=100)
    # plt.show()

#---------------------------------------------------------------------
# plot hbcount and hbtime per tid (each tid has 2 figures, 
# one hbcount and one for hbtime)
#---------------------------------------------------------------------
# plot hbcount and hbtime for all ranks(each rank has 2 figures, one hbcount and one for hbtime)
def plotPerTID(path, ranks, numofHBs, hbNames, outPath):
    for pid in ranks.keys():
        df =  extractDataFromCSV(path + "appekg-" + str(pid) + ".csv")
        df = pd.concat([df, df])
        rank = ranks[pid]
        threadIDs = extractTHreadIDS(df)
    for j in threadIDs:
        plotHBCount(df.loc[df['threadID'] == j], numofHBs, hbNames, rank, j, outPath, "per-tid")
        plotHBDuration(df.loc[df['threadID'] == j], numofHBs,hbNames, rank, j, outPath, "per-tid")

#---------------------------------------------------------------------
# plot hbcount and hbtime per tid per pid 
#---------------------------------------------------------------------
def plotPerTIDPerPID(path, ranks, numofHBs, hbNames, outPath):
    for pid in ranks.keys():
        df =  extractDataFromCSV(path + "appekg-" + str(pid) + ".csv")
        rank = ranks[pid]
        threadIDs = extractTHreadIDS(df)
        for j in threadIDs:
            plotHBCount(df.loc[df['threadID'] == j], numofHBs, hbNames, rank, j, outPath, "per-tid-pid")
            plotHBDuration(df.loc[df['threadID'] == j], numofHBs,hbNames, rank, j, outPath, "per-tid-pid")

#---------------------------------------------------------------------
# plot hbcount and hbtime for all ranks(each rank has 2 figures, 
# one hbcount and one for hbtime)
#---------------------------------------------------------------------
def plotPerPID(path, ranks, numofHBs, hbNames, outPath):
    # TODO: either all ranks or a specific rank
    for pid in ranks.keys():
        df =  extractDataFromCSV(path + "appekg-" + str(pid) + ".csv")
        rank = ranks[pid]
        plotHBCount(df, numofHBs, hbNames, rank, -1, outPath, "per-pid")
        plotHBDuration(df, numofHBs,hbNames, rank, -1,outPath, "per-pid")

#---------------------------------------------------------------------
# main
#---------------------------------------------------------------------
parser = argparse.ArgumentParser(prog="plotting", description = 'Plotting Heartbeat Data')
# define how a single command-line argument should be parsed.
parser.add_argument('--input', '-i', type=str, required=True, help = "Heartbeat data path.")
parser.add_argument('--type', '-t', type=str, required=False, choices = ['per-pid', 'per-tid', 'per-tid-pid'], help="Select what plots to generate. Per processID, Per ThreadID or per Per ThreadID Per ProcessID. Default is all.")
parser.add_argument('--output', '-o', type=str, required=False, help = "Output directory path. If not defined, plots will be saved in the input path.")
# create a new ArgumentParser object
args = parser.parse_args()
inputPath = args.input 
if not inputPath.endswith('/'):
    inputPath += '/'
if not os.path.exists(inputPath):
    print("No such directory, use a valid input path.")
if args.output == None:
    outputPath = args.input
else:
    outputPath = args.output
    if not outputPath.endswith('/'):
        outputPath += '/'
if not os.path.exists(outputPath):
    os.makedirs(outputPath)
# find csv files 
csvFiles = list_files(inputPath, ".csv")
# find json files
jsonFiles = list_files(inputPath, ".json")
if not jsonFiles or not csvFiles:
    print("The input directory has no input files")
    exit(1)
jsonfile = open(inputPath + jsonFiles[0],"r")
data = json.load(jsonfile)
# select one of the csv files to extract # of HBs and HBs names
df = extractDataFromCSV(inputPath + csvFiles[0])
ranks, numofHBs,hbNames = createHBNames(inputPath, jsonFiles, df)
colors = ['r', 'g', 'b', 'y', 'k','c','m']
# plot data (per-pid, per-tid or per-tid-pid)
if pltType == "per-pid":
    plotPerPID(inputPath, ranks, numofHBs, hbNames, outputPath)
elif pltType == "per-tid":
    plotPerTID(inputPath, ranks, numofHBs, hbNames, outputPath)
elif pltType == "per-tid-pid":
    plotPerTIDPerPID(inputPath, ranks, numofHBs, hbNames, outputPath)
else:
    plotPerPID(inputPath, ranks, numofHBs, hbNames, outputPath)
    plotPerTIDPerPID(inputPath, ranks, numofHBs, hbNames, outputPath)
    plotPerTID(inputPath, ranks, numofHBs, hbNames, outputPath)