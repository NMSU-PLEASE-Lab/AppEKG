#
# stats.py
#
# This script prints out statistics per tid/timesec and pid/tid in a csv format. These statistics consist of min, max, std, mean, median, range, Q1, Q2, Q3, skew, kurtosis, count.
# To run the script, please use python3 and run 'python3 stats.py --help' for more instruction on how to run it.
# 

import pandas as pd
import sys
from os import listdir
import numpy as np
import argparse 

# Find files by extension
def list_files(dir_path, ext):
    files = listdir(dir_path)
    return [csv for csv in files if csv.endswith(ext)]

# Reference: https://medium.com/@pritul.dave/everything-about-moments-skewness-and-kurtosis-using-python-numpy-df305a193e46
def fun_skewness(arr):
    mean_ = np.mean(arr)
    median_ = np.median(arr)
    std_ = np.std(arr)
    if (std_ == 0):
        return np.nan
    skewness = 3*(mean_ - median_) / std_
    return skewness

# Reference: https://medium.com/@pritul.dave/everything-about-moments-skewness-and-kurtosis-using-python-numpy-df305a193e46
def fun_kurtosis(arr):
    mean_ = np.mean(arr)
    median_ = np.median(arr)
    mu4 = np.mean((arr - mean_)**4)
    mu2 = np.mean((arr-mean_)**2)
    if (mu2 == 0):
        return np.nan
    beta2 = mu4 / (mu2**2)
    gamma2 = beta2 - 3
    return gamma2

# extracte the unique thread IDs from the dataframe
def getThreadsID(df):
    threadsIDs = df.threadID.unique()
    threadsIDs.sort()
    return threadsIDs

# extracte the unique thread IDs from the dataframe
def getTimesecs(df):
    timesecs = df.timemsec.unique()
    timesecs.sort()
    return timesecs

# Print statistics function
def print_stats_per_thread(df):
    timesec = getTimesecs(df)
    threadsIDs = getThreadsID(df)
    print ("{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:}".format('timemsec','threadID','field','min','max','std','mean','median','range','Q1','Q2','Q3','skew','kurtosis','count'))
    for time in timesec:
        for thread in threadsIDs:
            _df = df[(df["threadID"] == thread) & (df["timemsec"] == time)].copy()
            _df.drop("timemsec", axis=1, inplace=True)
            _df.drop("threadID", axis=1, inplace=True)
            fields = _df.columns
            for f in fields:
                _count = _df.shape[0]
                _max = _df[f].max()
                _min = _df[f].min()
                _std = _df[f].std()
                _mean = _df[f].mean()
                _median = _df[f].median()
                _range = _max - _min
                _quantile1 = _df[f].quantile(q=0.25)
                _quantile2 = _df[f].quantile(q=0.5)
                _quantile3 = _df[f].quantile(q=0.75)
                _skew = fun_skewness(_df[f].tolist())
                _kurtosis = fun_kurtosis(_df[f].tolist())
                print ("{:},{:},{:},{:},{:3f},{:3f},{:3f},{:3f},{:3f},{:3f},{:3f},{:3f},{:3f},{:},{:}".format(time, thread, f, _min,_max,_std,_mean,_median,_range,_quantile1,_quantile2,_quantile3,_skew,_kurtosis,_count))


# Print statistics for each thread
def print_stats_per_process(df):
        print ("{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:},{:}".format('threadID','field','min','max','std','mean','median','range','Q1','Q2','Q3','skew','kurtosis','count'))
        threadsIDs = getThreadsID(df)
        for t  in threadsIDs:
            _df = df[df["threadID"] == t].copy()
            _df.drop("timemsec", axis=1, inplace=True)
            _df.drop("threadID", axis=1, inplace=True)
            fields = _df.columns
            for f in fields:
                _count = _df.shape[0]
                _max = _df[f].max()
                _min = _df[f].min()
                _std = _df[f].std()
                _mean = _df[f].mean()
                _median = _df[f].median()
                _range = _max - _min
                _quantile1 = _df[f].quantile(q=0.25)
                _quantile2 = _df[f].quantile(q=0.5)
                _quantile3 = _df[f].quantile(q=0.75)
                _skew = fun_skewness(_df[f].tolist())
                _kurtosis = fun_kurtosis(_df[f].tolist())
                print ("{:},{:},{:},{:},{:3f},{:3f},{:3f},{:3f},{:3f},{:3f},{:3f},{:3f},{:3f},{:}".format(t, f, _min,_max,_std,_mean,_median,_range,_quantile1,_quantile2,_quantile3,_skew,_kurtosis,_count))




# Parse arguments
parser = argparse.ArgumentParser(prog="stats.py", description='This script prints out statistics per tid/timemsec and per pid/tid in csv format. These statistics consist of min, max, std, mean, median, range, Q1, Q2, Q3, skew, kurtosis, count.')

# Define how a single command-line argument should be parsed.
parser.add_argument('--input', '-i', type=str, required=True, help="Input directory path. This directory should contain only data of a single run.")
parser.add_argument('--type', '-t', type=str, required=True, choices=['per-tid-timemsec','per-pid-tid'], help="Select what statistics to generate. Per threadID/timemsec or per processID/threadID.")

# Create a new ArgumentParser object.
args = parser.parse_args()
dir_path = args.input + '/'
type = args.type

# Find CSV files
csv_files = list_files(dir_path, ".csv")

# Empty DataFrame
df = pd.DataFrame()

# Merge CSV files and
for csv in csv_files:
    csv_path = dir_path + csv
    _df = pd.read_csv(csv_path)
    df = pd.concat([df, _df])
    # Print per processID/threadID
    if type == 'per-pid-tid':
        print(csv)
        print_stats_per_process(df)

# Print per threadID/timemsec
if type == 'per-tid-timemsec':
    print_stats_per_thread(df)


