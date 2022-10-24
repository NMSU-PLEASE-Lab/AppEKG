#
# DO NOT USE
# NOTE: this is an old script that processed per-process data. 
#

# Script to get stats per interval
# module load python/351
# python per_interval_stats.py <path> <#HB> > <output_path>
# python per_interval_stats.py /project/hpcjobquality/testruns/hbeatData/miniamr/ 6 > <output_path>

mport pandas as pd
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
    skewness = 3*(mean_ - median_) / std_
    return skewness

# Reference: https://medium.com/@pritul.dave/everything-about-moments-skewness-and-kurtosis-using-python-numpy-df305a193e46
def fun_kurtosis(arr):
    mean_ = np.mean(arr)
    median_ = np.median(arr)
    mu4 = np.mean((arr - mean_)**4)
    mu2 = np.mean((arr-mean_)**2)
    beta2 = mu4 / (mu2**2)
    gamma2 = beta2 - 3
    return gamma2

# Print statistics for each HB
def print_stats(df, nHB, timesec):
        print(timesec, end='')
        for hb in range(1,nHB+1):
                col = "hbeat" + str(hb)
                _df = df[df[col] != 0]
                _max = _df[col].max()
                _min = _df[col].min()
                _mean = _df[col].mean()
                _range = _max - _min
                _quantile1 = _df[col].quantile(q=0.25)
                _quantile2 = _df[col].quantile(q=0.5)
                _quantile3 = _df[col].quantile(q=0.75)
                _skew = fun_skewness(_df[col].tolist())
                _kurtosis = fun_kurtosis(_df[col].tolist())

                print (",{},{},{:.3f},{},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f}".format(_min,_max,_mean,_range,_quantile1,_quantile2,_quantile3,_skew,_kurtosis), end='')
        print("")

# Read arguments
dir_path = sys.argv[1]
nHB = int(sys.argv[2])

# Find CSV files
csv_files = list_files(dir_path, ".csv")

# Empty DataFrame
df = pd.DataFrame()

# Collect statistics from each CSV
for csv in csv_files:
        csv_path = dir_path + csv
        _df = pd.read_csv(csv_path)
        df = pd.concat([df, _df])

# Colles all the intervals
timesec = df['timesec'].unique()

# Print CSV header
print ("{}".format("timesec"), end='')
for hb in range(1,nHB+1):
        print(",{},{},{},{},{},{},{},{},{}".format("min" + str(hb),"max" + str(hb),"mean" + str(hb),"range" + str(hb),"Q1-" + str(hb),"Q2-" + str(hb),"Q3-" + str(hb),"skew" + str(hb),"kurtosis" + str(hb)), end='')
print("")

# Print statistics for each interval
for t in timesec:
        _df = df[df["timesec"] == t]
        print_stats(_df,nHB,t)
