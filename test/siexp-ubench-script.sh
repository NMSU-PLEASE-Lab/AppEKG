#!/bin/bash

# Experimental runs for AppeKG overhead analysis using the ubench 
# microbenchmark
#
# The time values are output in a CSV format
# so that they can be grabbed easily into a spreadsheet. You can do
#  "grep time- <log output filename>"

#SBATCH --job-name appekg-ubench-siexp # name that will show up in the queue
#SBATCH --output appekg-ubench-siexp-%j.out   # %j is job id
#SBATCH --exclusive        # request exclusive access to nodes
#SBATCH --nodes=1          # number of nodes to use to restrict tasks to
#SBATCH --ntasks=1         # number of processes (MPI ranks) to run
#SBATCH --cpus-per-task=32 # the number of threads allocated to each process
#SBATCH --mem-per-cpu=100M  # memory per CPU core (thread)
#SBATCH --partition=normal # the partitions to run in (comma seperated)
#SBATCH --time=0-08:00:00  # max time for analysis (day-hour:min:sec)
#SBATCH --mail-user joncook@nmsu.edu  # your email address
#SBATCH --mail-type END    # slurm will email you when your job ends

# make this same as cpus-per-task, or less
export OMP_NUM_THREADS=32

# executables (the sampling interval exp will use hb10)
EXEC_CLEAN=/fs1/home/joncook/ws/AppEKG/test/ubench-clean
EXEC_APPEKG1=/fs1/home/joncook/ws/AppEKG/test/ubench-hb1
EXEC_APPEKG10=/fs1/home/joncook/ws/AppEKG/test/ubench-hb10
EXEC_APPEKG100=/fs1/home/joncook/ws/AppEKG/test/ubench-hb100
EXEC_APPEKG1000=/fs1/home/joncook/ws/AppEKG/test/ubench-hb1000

# not used for now
EXEC_ARGS="-l 1024"

# make subdirs for appekg data for each sampling interval, so the data is
# kept separate; we can verify our sampling intervals by looking at the data
mkdir -p SI1
mkdir -p SI5
mkdir -p SI10
mkdir -p SI60

# 1st Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_SAMPLING_INTERVAL=1
export APPEKG_OUTPUT_PATH=SI1
/usr/bin/time -f 'time-appekg-si1,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=5
export APPEKG_OUTPUT_PATH=SI5
/usr/bin/time -f 'time-appekg-si5,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=10
export APPEKG_OUTPUT_PATH=SI10
/usr/bin/time -f 'time-appekg-si10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=60
export APPEKG_OUTPUT_PATH=SI60
/usr/bin/time -f 'time-appekg-si60,%U,%S,%e' srun $EXEC_APPEKG10

# 2nd Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_SAMPLING_INTERVAL=1
export APPEKG_OUTPUT_PATH=SI1
/usr/bin/time -f 'time-appekg-si1,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=5
export APPEKG_OUTPUT_PATH=SI5
/usr/bin/time -f 'time-appekg-si5,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=10
export APPEKG_OUTPUT_PATH=SI10
/usr/bin/time -f 'time-appekg-si10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=60
export APPEKG_OUTPUT_PATH=SI60
/usr/bin/time -f 'time-appekg-si60,%U,%S,%e' srun $EXEC_APPEKG10

# 3rd Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_SAMPLING_INTERVAL=1
export APPEKG_OUTPUT_PATH=SI1
/usr/bin/time -f 'time-appekg-si1,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=5
export APPEKG_OUTPUT_PATH=SI5
/usr/bin/time -f 'time-appekg-si5,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=10
export APPEKG_OUTPUT_PATH=SI10
/usr/bin/time -f 'time-appekg-si10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=60
export APPEKG_OUTPUT_PATH=SI60
/usr/bin/time -f 'time-appekg-si60,%U,%S,%e' srun $EXEC_APPEKG10

# 4th Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_SAMPLING_INTERVAL=1
export APPEKG_OUTPUT_PATH=SI1
/usr/bin/time -f 'time-appekg-si1,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=5
export APPEKG_OUTPUT_PATH=SI5
/usr/bin/time -f 'time-appekg-si5,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=10
export APPEKG_OUTPUT_PATH=SI10
/usr/bin/time -f 'time-appekg-si10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=60
export APPEKG_OUTPUT_PATH=SI60
/usr/bin/time -f 'time-appekg-si60,%U,%S,%e' srun $EXEC_APPEKG10

# 5th Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_SAMPLING_INTERVAL=1
export APPEKG_OUTPUT_PATH=SI1
/usr/bin/time -f 'time-appekg-si1,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=5
export APPEKG_OUTPUT_PATH=SI5
/usr/bin/time -f 'time-appekg-si5,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=10
export APPEKG_OUTPUT_PATH=SI10
/usr/bin/time -f 'time-appekg-si10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_SAMPLING_INTERVAL=60
export APPEKG_OUTPUT_PATH=SI60
/usr/bin/time -f 'time-appekg-si60,%U,%S,%e' srun $EXEC_APPEKG10

