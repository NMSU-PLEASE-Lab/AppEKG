# AppEKG: A Heartbeat Framework

__NOTE:__ _AppEKG is currently being developed in a private GitLab project...the initial public version will be coming before Nov 2022! Current imports do not yet have updated analysis tools and their documentation._

AppEKG is a heartbeat instrumentation library and analysis framework,
enabling the tracking and analysis of application performance at the
application phase level with a low-overhead, production-usable 
method.

The defined macros are the preferred way of creating AppEKG
instrumentation in an application, but the underlying functions
are also available in the API; the functions for begin/end
heatbeats do not inherently support a rate factor, however. 

The macro interface is:  
- EKG_BEGIN_HEARTBEAT(id, rateFactor) 
- EKG_END_HEARTBEAT(id) 
- EKG_PULSE_HEARTBEAT(id, rateFactor) 
- EKG_INITIALIZE(numHeartbeats, samplingInterval, appid, jobid, rank, silent) 
- EKG_FINALIZE() 
- EKG_DISABLE() 
- EKG_ENABLE() 
- EKG_NAME_HEARTBEAT(id, name) 
- EKG_IDOF_HEARTBEAT(name) 
- EKG_NAMEOF_HEARTBEAT(id) 

Heartbeat IDs are small integers starting at 1, and must be sequential.
A unique heartbeat ID is meant to represent a particular phase or kernel 
of the application, and generally each instrumentation site has a unique
heartbeat ID.

_rateFactor_ controls the speed of heartbeat production, if an instrumentation
site is invoked too frequently. A rateFactor of 100, for example, would 
produce a heartbeat once every 100 executions of the instrumentation site.

AppEKG initialization accepts as parameters the number of unique heartbeats
(maximum heartbeat ID), the number of seconds between data samples, a
unique application ID, job ID, MPI rank, and a silent flag for turning
off (unlikely but possible) stderr messages. If the job ID is left 0, PBS
and Slurm environment variables are checked and used to set it. An 
application ID is useful when creating a historical database of multiple
applications' data.

Heartbeats can be given a name using the API; names should generally
refer to the conceptual meaning of the application phase or kernel the
heartbeat is capturing.

Environment Variables:  
- APPEKG_SAMPLING_INTERVAL : integer, number of seconds between samples; 
                             will override AppEKG initialization parameter
- PBS_JOBID : if found, used for the 'jobid' data field, if param jobid=0
- SLURM_JOB_ID : if found, used for the 'jobid' data field, if param jobid=0

## Building

_make_ should work in the main directory; _make doc_ will create doxygen
documentation. If you need to select optional output modes such as LDMS
streams, edit the Makefile to set it up properly.

In the _test_ directory, 'make' will build a variety of tests. The test 
_evensum_ is the most complete, using OpenMP threads to generate heartbeats
per thread.

## Running an application

In the _test_ directory, running the _evensum_ executable will produce two
output files one named 'appekg-###.json' and one named 'appekg-###.csv'. 
The number in the filename is the PID of the process (on a cluster, each
process will produce its own heartbeat data files).

The JSON file is the metadata for the heartbeat data, and includes a variety
of data fields; most are self-explanatory. The field _component_ is a number
that is extracted from the host name, if the name has a number in it (most HPC
clusters set up their computational nodes with names with ID numbers in them).

The CSV files is the heartbeat data, in column format. The first two columns
are a timestamp (end of sampling interval, milliseconds since the beginning 
of the execution) and a thread ID (a unique, small but non-consecutive integer
value). The rest of the columns is heartbeat data, two columns per heartbeat;
the first is the number of heartbeats that occurred in this sampling interval,
the second is the average duration of these heartbeats (microseconds).

AppEKG does keep heartbeat data per thread, using hashing to quickly locate
the thread data region; hash collisions can cause threads to be ignored, and
the hash table is static in size and will not grow. The default size is 57 but
can be changed by editing _appekg.h_ (around line 90). 

## Running analyses

TO DO.

