# Script to get stats per timesecs/threadID from appekg.csv files
#
# To run this script, please use python 3
# python per_interval_thread_stats.py <path_to_appekg.csv_files> > <output_path>

import pandas as pd
import sys
from os import listdir
import numpy as np

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

# Print statistics
def print_stats(df):
    timesec = getTimesecs(df)
    threadsIDs = getThreadsID(df)
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
                print ("{:<10} {:<10} {:<10} {:<10} {:<10.3f} {:<10.3f} {:<10.3f} {:<10.3f} {:<10.3f} {:<10.3f} {:<10.3f} {:<10.3f} {:<10.3f} {:<10}".format(time, thread, f, _min,_max,_std,_mean,_median,_range,_quantile1,_quantile2,_quantile3,_skew,_kurtosis,_count))

# Read arguments
dir_path = sys.argv[1]

# Find CSV files
csv_files = list_files(dir_path, ".csv")

# Empty DataFrame
df = pd.DataFrame()

# Merge CSV files
for csv in csv_files:
        csv_path = dir_path + csv
        _df = pd.read_csv(csv_path)
        df = pd.concat([df, _df])

# Collects all the intervals and threads

# Print CSV
print ("{:<10}{:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10}".format('timemsec','threadID','field','min','max','std','mean','median','range','Q1','Q2','Q3','skew','kurtosis','count'))
print_stats(df)
