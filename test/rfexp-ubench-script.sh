#!/bin/bash

# Experimental runs for AppeKG overhead analysis using the ubench 
# microbenchmark
#
# The time values are output in a CSV format
# so that they can be grabbed easily into a spreadsheet. You can do
#  "grep time- <log output filename>"

#SBATCH --job-name appekg-ubench-rfexp # name that will show up in the queue
#SBATCH --output appekg-ubench-rfexp-%j.out   # %j is job id
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

# executables 
EXEC_CLEAN=/fs1/home/joncook/ws/AppEKG/test/ubench-clean
EXEC_APPEKG1=/fs1/home/joncook/ws/AppEKG/test/ubench-hb1
EXEC_APPEKG10=/fs1/home/joncook/ws/AppEKG/test/ubench-hb10
EXEC_APPEKG100=/fs1/home/joncook/ws/AppEKG/test/ubench-hb100
EXEC_APPEKG1000=/fs1/home/joncook/ws/AppEKG/test/ubench-hb1000

# not used for now
EXEC_ARGS="-l 1024"

# sampling (and file output) interval, seconds
export APPEKG_SAMPLING_INTERVAL=1

# make subdirs for appekg data for each sampling interval, so the data is
# kept separate; we can verify our sampling intervals by looking at the data
mkdir -p RF1
mkdir -p RF10
mkdir -p RF100
mkdir -p RF1000

# 1st Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_OUTPUT_PATH=RF1
/usr/bin/time -f 'time-appekg-rf1,%U,%S,%e' srun $EXEC_APPEKG1
export APPEKG_OUTPUT_PATH=RF10
/usr/bin/time -f 'time-appekg-rf10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_OUTPUT_PATH=RF100
/usr/bin/time -f 'time-appekg-rf100,%U,%S,%e' srun $EXEC_APPEKG100
export APPEKG_OUTPUT_PATH=RF1000
/usr/bin/time -f 'time-appekg-rf1000,%U,%S,%e' srun $EXEC_APPEKG1000

# 2nd Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_OUTPUT_PATH=RF1
/usr/bin/time -f 'time-appekg-rf1,%U,%S,%e' srun $EXEC_APPEKG1
export APPEKG_OUTPUT_PATH=RF10
/usr/bin/time -f 'time-appekg-rf10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_OUTPUT_PATH=RF100
/usr/bin/time -f 'time-appekg-rf100,%U,%S,%e' srun $EXEC_APPEKG100
export APPEKG_OUTPUT_PATH=RF1000
/usr/bin/time -f 'time-appekg-rf1000,%U,%S,%e' srun $EXEC_APPEKG1000

# 3rd Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_OUTPUT_PATH=RF1
/usr/bin/time -f 'time-appekg-rf1,%U,%S,%e' srun $EXEC_APPEKG1
export APPEKG_OUTPUT_PATH=RF10
/usr/bin/time -f 'time-appekg-rf10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_OUTPUT_PATH=RF100
/usr/bin/time -f 'time-appekg-rf100,%U,%S,%e' srun $EXEC_APPEKG100
export APPEKG_OUTPUT_PATH=RF1000
/usr/bin/time -f 'time-appekg-rf1000,%U,%S,%e' srun $EXEC_APPEKG1000

# 4th Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_OUTPUT_PATH=RF1
/usr/bin/time -f 'time-appekg-rf1,%U,%S,%e' srun $EXEC_APPEKG1
export APPEKG_OUTPUT_PATH=RF10
/usr/bin/time -f 'time-appekg-rf10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_OUTPUT_PATH=RF100
/usr/bin/time -f 'time-appekg-rf100,%U,%S,%e' srun $EXEC_APPEKG100
export APPEKG_OUTPUT_PATH=RF1000
/usr/bin/time -f 'time-appekg-rf1000,%U,%S,%e' srun $EXEC_APPEKG1000

# 5th Run
/usr/bin/time -f 'time-clean,%U,%S,%e' srun $EXEC_CLEAN
export APPEKG_OUTPUT_PATH=RF1
/usr/bin/time -f 'time-appekg-rf1,%U,%S,%e' srun $EXEC_APPEKG1
export APPEKG_OUTPUT_PATH=RF10
/usr/bin/time -f 'time-appekg-rf10,%U,%S,%e' srun $EXEC_APPEKG10
export APPEKG_OUTPUT_PATH=RF100
/usr/bin/time -f 'time-appekg-rf100,%U,%S,%e' srun $EXEC_APPEKG100
export APPEKG_OUTPUT_PATH=RF1000
/usr/bin/time -f 'time-appekg-rf1000,%U,%S,%e' srun $EXEC_APPEKG1000

