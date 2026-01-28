#!/bin/bash

# Experimental runs for AppeKG overhead analysis using the ubench 
# microbenchmark
#
# The time values are output in a CSV format
# so that they can be grabbed easily into a spreadsheet. You can do
#  "grep time- <log output filename>"

#SBATCH --job-name appekg-ubench-ttexp # name that will show up in the queue
#SBATCH --output appekg-ubench-ttexp-%j.out   # %j is job id
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
EXEC_APPEKG57=/fs1/home/joncook/ws/AppEKG/test/ubench-hb10
EXEC_APPEKG1=/fs1/home/joncook/ws/AppEKG/test/ubench-tt1
EXEC_APPEKG3=/fs1/home/joncook/ws/AppEKG/test/ubench-tt3
EXEC_APPEKG9=/fs1/home/joncook/ws/AppEKG/test/ubench-tt9
EXEC_APPEKG15=/fs1/home/joncook/ws/AppEKG/test/ubench-tt15

EXEC_ARGS="-i 500"

export APPEKG_SAMPLING_INTERVAL=1

# make subdirs for appekg data for each sampling interval, so the data is
# kept separate; we can verify our sampling intervals by looking at the data
mkdir -p TT1
mkdir -p TT3
mkdir -p TT9
mkdir -p TT15
mkdir -p TT57

# 1st Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT1
/usr/bin/time -f 'time-appekg-tt1,%U,%S,%e' srun $EXEC_APPEKG1 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT3
/usr/bin/time -f 'time-appekg-tt4,%U,%S,%e' srun $EXEC_APPEKG3 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT9
/usr/bin/time -f 'time-appekg-tt9,%U,%S,%e' srun $EXEC_APPEKG9 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT15
/usr/bin/time -f 'time-appekg-tt15,%U,%S,%e' srun $EXEC_APPEKG15 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT57
/usr/bin/time -f 'time-appekg-tt57,%U,%S,%e' srun $EXEC_APPEKG57 $EXEC_ARGS

# 2nd Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT1
/usr/bin/time -f 'time-appekg-tt1,%U,%S,%e' srun $EXEC_APPEKG1 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT3
/usr/bin/time -f 'time-appekg-tt4,%U,%S,%e' srun $EXEC_APPEKG3 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT9
/usr/bin/time -f 'time-appekg-tt9,%U,%S,%e' srun $EXEC_APPEKG9 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT15
/usr/bin/time -f 'time-appekg-tt15,%U,%S,%e' srun $EXEC_APPEKG15 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT57
/usr/bin/time -f 'time-appekg-tt57,%U,%S,%e' srun $EXEC_APPEKG57 $EXEC_ARGS

# 3rd Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT1
/usr/bin/time -f 'time-appekg-tt1,%U,%S,%e' srun $EXEC_APPEKG1 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT3
/usr/bin/time -f 'time-appekg-tt4,%U,%S,%e' srun $EXEC_APPEKG3 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT9
/usr/bin/time -f 'time-appekg-tt9,%U,%S,%e' srun $EXEC_APPEKG9 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT15
/usr/bin/time -f 'time-appekg-tt15,%U,%S,%e' srun $EXEC_APPEKG15 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT57
/usr/bin/time -f 'time-appekg-tt57,%U,%S,%e' srun $EXEC_APPEKG57 $EXEC_ARGS

# 4th Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT1
/usr/bin/time -f 'time-appekg-tt1,%U,%S,%e' srun $EXEC_APPEKG1 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT3
/usr/bin/time -f 'time-appekg-tt4,%U,%S,%e' srun $EXEC_APPEKG3 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT9
/usr/bin/time -f 'time-appekg-tt9,%U,%S,%e' srun $EXEC_APPEKG9 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT15
/usr/bin/time -f 'time-appekg-tt15,%U,%S,%e' srun $EXEC_APPEKG15 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT57
/usr/bin/time -f 'time-appekg-tt57,%U,%S,%e' srun $EXEC_APPEKG57 $EXEC_ARGS

# 5th Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT1
/usr/bin/time -f 'time-appekg-tt1,%U,%S,%e' srun $EXEC_APPEKG1 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT3
/usr/bin/time -f 'time-appekg-tt4,%U,%S,%e' srun $EXEC_APPEKG3 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT9
/usr/bin/time -f 'time-appekg-tt9,%U,%S,%e' srun $EXEC_APPEKG9 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT15
/usr/bin/time -f 'time-appekg-tt15,%U,%S,%e' srun $EXEC_APPEKG15 $EXEC_ARGS
export APPEKG_OUTPUT_PATH=TT57
/usr/bin/time -f 'time-appekg-tt57,%U,%S,%e' srun $EXEC_APPEKG57 $EXEC_ARGS

