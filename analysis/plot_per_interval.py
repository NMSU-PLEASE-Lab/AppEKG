# I used Jypiter NB with Ondemand.
# could be used as CLI argument with small adjustments
# python plot_per_interval.py <csv path> <hb count>
import pandas as pd
import sys
import numpy as np
import matplotlib.pyplot as plt

# Read arguments
#csv_path = sys.argv[1]
#nHB = int(sys.argv[2])
nHB = 3
line_colors = ["#fc0f03","#104E8B","#FFD700","#595959", "#FF69B4", "#FF8000"]
shade_colors = ["#fc2403","#1E90FF","#CDAD00","#7F7F7F","#CD6090","#CD8500" ]

# Read CSV
#df = pd.read_csv(csv_path, na_values=0)
df = pd.read_csv('/project/hpcjobquality/testruns/fse2022data/stats-strahinja/miniFE-med/per_interval_stats-miniFE-med.csv',na_values=0)

# Figure settings
fig_size = (8,4.5)
x_label = "Time (sec)"
y_label = "Interval Heartbeat Count"

# One plot for HB
for i in range(1,nHB+1):
    _min = "min" + str(i)
    _max = "max" + str(i)
    _mean = "mean" + str(i)
    #cols = [_mean]
    df.plot(x="timesec", y=_mean, logy=True, figsize=fig_size, xlabel=x_label, ylabel=y_label, title="Heartbeat " + str(i), color=line_colors[i])
    plt.fill_between(x=df["timesec"], y1=df[_min] , y2=df[_max], alpha=.25, linewidth=0, color=shade_colors[i])
    plt.legend(bbox_to_anchor=(1.0, 1.0))
    plt.savefig("hb-" + str(i) + ".png", bbox_inches="tight", dpi=300)
    plt.show()
'''
# All max plot
cols = []
for i in range(1,nHB+1):
    cols.append("max" + str(i))
df.plot(x="timesec", y=cols, logy=True, figsize=fig_size, xlabel=x_label, ylabel=y_label, title="Heartbeats Maximum", color=line_colors)
plt.legend(bbox_to_anchor=(1.0, 1.0))
plt.savefig("hb-max.png", bbox_inches="tight", dpi=300)
plt.show()

# All min plot
cols=[]
for i in range(1,nHB+1):
    cols.append("min" + str(i))
df.plot(x="timesec", y=cols, logy=True, figsize=fig_size, xlabel=x_label, ylabel=y_label, title="Heartbeats Minimum", color=line_colors)
plt.legend(bbox_to_anchor=(1.0, 1.0))
plt.savefig("hb-min.png", bbox_inches="tight", dpi=300)
plt.show()

# All mean plot
cols =[]
for i in range(1,nHB+1):
    cols.append("mean" + str(i))
df.plot(x="timesec", y=cols, logy=True, figsize=fig_size, xlabel=x_label, ylabel=y_label, title="Heartbeats Mean", color=line_colors)
plt.legend(bbox_to_anchor=(1.0, 1.0))
plt.savefig("hb-mean.png", bbox_inches="tight", dpi=300)
plt.show()
'''
# Global plot
ax_= plt.gca()
for i in range(1,nHB+1):
    _min = "min" + str(i)
    _max = "max" + str(i)
    _mean = "mean" + str(i)
    df.plot(ax=ax_, x="timesec", y=_mean, logy=True, figsize=fig_size, xlabel=x_label, ylabel=y_label, title="Heartbeats all", color=line_colors[i])
    plt.fill_between(x=df["timesec"], y1=df[_min] , y2=df[_max], alpha=.25, linewidth=0, color=shade_colors[i])
plt.legend(bbox_to_anchor=(1.0, 1.0))
plt.savefig("hb-all.png", bbox_inches="tight", dpi=300)
plt.show()
