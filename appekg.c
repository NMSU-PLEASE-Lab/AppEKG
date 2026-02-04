/**
* \file appekg.c
* \brief AppEKG Implementation
* 
* This current implementation supports two modes: storing data into 
* CSV files or using LDMS Streams to collect data. Working to add
* SQLite3 support.
*
* Environment variables -- see appekg.h header comment
*  
* Threading: basic idea: hash thread ID into a small range, use this
* as index into array of thread heartbeat data. If hash collides, ignore
* the 2nd+ threads and don't collect data on them.
**/

#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

#define EKG_EXTERN
#include "appekg.h"

#ifdef INCLUDE_LDMS
#include "ldms/ldms_appstreams.h"
#endif

#define MAX_JSON_STRLEN 2048
#define FILENAME_PREFIX "appekg"

//--------------------------------------------------------------------
// macros for getting thread ID; choices are either OpenMP or
// Pthreads. We do +1 so that we don't start at 0 for OpenMP, but
// since it MUST match the +1 in the macros EKG_BEGIN/END/PULSE_HB,
// we must do it in all macros (pthreads, too).
#ifdef EKG_USE_OPENMP
#include <omp.h>
#define THREAD_ID ((omp_get_thread_num()+1)%EKG_MAX_THREADS)
#define THREAD_ID_FULL (omp_get_thread_num()+1)
#define THREAD_ID_FUNC omp_get_thread_num
#else
#define THREAD_ID (((unsigned int)pthread_self()+1) % EKG_MAX_THREADS)
#define THREAD_ID_FULL ((unsigned int)pthread_self()+1)
#define THREAD_ID_FUNC pthread_self
#endif
//--------------------------------------------------------------------

static enum {
    APPEKG_STAT_OK = 0,
    APPEKG_STAT_ERROR,
    APPEKG_STAT_DISABLED
} appekgStatus = APPEKG_STAT_DISABLED; /** status of APPEKG **/

// Output modes: Default CSV should always be compiled in as an
// available option; others should be ifdef'd to be selected for
// compilation; still need to create an output selection mode (and
// may need to move this into appekg.h for macro availability)
static enum {
    DO_NO_OUTPUT = 0,
    DO_CSV_FILES,
#ifdef INCLUDE_LDMS
    DO_LDMS_STREAMS,
#endif
#ifdef INCLUDE_SQLITE
    DO_SQLITE,
#endif
    DO_UKN_OUTPUT
} ekgOutputMode = DO_CSV_FILES;

// Metadata generic metric structure; this is left over from
// some code that was integrated into LDMS, not sure if we should
// abandon it or not. We could certainly simplify it quite a bit
// by not supporting all the data types we aren't using.
#define MAX_BASE_METRICS 10
static struct Metric {
    char* name;
    enum {
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        INT8,
        INT16,
        INT32,
        INT64,
        FLOAT,
        DOUBLE,
        STRING
    } type;
    union {
        uint8_t u8val;
        uint16_t u16val;
        uint32_t u32val;
        uint64_t u64val;
        int8_t i8val;
        int16_t i16val;
        int32_t i32val;
        int64_t i64val;
        float fpval;
        double dpval;
        //char    strval[64];  // leave out until we need it, too much space
    } v;
} baseMetrics[MAX_BASE_METRICS],
  threadMetrics[EKG_MAX_THREADS][EKG_MAX_HEARTBEATS * 2 + 2];

static int baseCount = 0;   /* number of base metrics */
static int metricCount = 0; /* number of metrics */

/* begin time for hbeats, per thread and per heartbeat, in microseconds */
static unsigned long beginHBTime[EKG_MAX_THREADS][EKG_MAX_HEARTBEATS] = {0};
static int numHeartbeats = 0; /* number of hbeats */
static char** hbNames;        // heartbeat names (used indices start at 1)

// thread id for the appekg sampling thread
static pthread_t samplingThreadId = 0;

static unsigned long samplingInterval = 1; /* in seconds */
static int doSampling = 0;
static void* performSampling(void* arg);
static int allowStderr = 0;
static pthread_mutex_t hblock;
static struct timespec programStartTime;
static unsigned int applicationID, jobID;
static char jsonFilename[1024];
static int numProcs = 1;
static int numNodes = 1;
static int userID;
static int rankID;
static int nodeID;

//
// We include the supporting source files here so that the shared data
// and functions can remain static and not show up in the library symbols
//
#include "csvoutput.c"
#include "ldmsoutput.c"
#include "sqliteoutput.c"

/**
 * \brief Initialize AppEKG
 *
 * All AppEKG initialization: data structures, job/run data, output files,
 * sampling thread startup, etc.
 *
 * \param appekgEnable enable data sampling 0-disable 1-enable.
 * \param appid is a unique identifying number for this application
 * \param jobid is the job id # (0 means look at PBS_JOBID env var)
 * \param rank is the MPI (or other) process rank designation
 * \param silent is nonzero if want no stderr
 * \return 0 on success. -1 is returned on failure.
 */
int ekgInitialize(unsigned int pNumHeartbeats, float pSamplingInterval,
                  unsigned int appid, unsigned int jobid, unsigned int rank,
                  unsigned int silent)
{
    int i;
    char* p;
    if (appekgStatus == APPEKG_STAT_OK)
        return 0; // other thread already did the init before we locked
    pthread_mutex_lock(&hblock);
    if (appekgStatus == APPEKG_STAT_OK) {
        pthread_mutex_unlock(&hblock);
        return 0; // other thread already did the init after we locked
    }
    allowStderr = !silent;
    applicationID = appid;
    rankID = rank;

    clock_gettime(CLOCK_REALTIME, &programStartTime);

    /* get number of hbeats from environment variable */
    // JEC: I disabled this, is just a built-in init param; this
    // should not be changeable in the environment
    //p = getenv("NO_OF_HBEATS");
    //numberOfHB = atoi(p);
    numHeartbeats = pNumHeartbeats;
    // set up heartbeat name array (index 0 is not used)
    hbNames = (char**)calloc(sizeof(char*), numHeartbeats + 1);

    /* check for sampling interval from environment variable */
    samplingInterval = pSamplingInterval;
    p = getenv("APPEKG_SAMPLING_INTERVAL");
    if (p)
        samplingInterval = strtoul(p, 0, 10);

    // Per thread heartbeat data; these are accessible in the
    // application instrumentation macros, and so their names
    // must be namespaced and not conflict with application names
    _ekgHBEndFlag = (unsigned int*)calloc(sizeof(unsigned int),
                                          (EKG_MAX_HEARTBEATS+1) * (EKG_MAX_THREADS+1));
    _ekgHBCount = (unsigned int*)calloc(sizeof(unsigned int),
                                        (EKG_MAX_HEARTBEATS+1) * (EKG_MAX_THREADS+1));
    _ekgActualThreadID =
          (unsigned int*)calloc(sizeof(unsigned int), EKG_MAX_THREADS+1);
    // TODO: make sure that the casting below is safe; I think it is
    _ekgThreadId = (long unsigned int (*)(void)) THREAD_ID_FUNC; 

    // Set up job id if needed: JEC - I changed this to override the
    // initialization argument rather than defer to it.
    int jid = 0;
    char* js = getenv("PBS_JOBID");
    if (js)
        jid = (int)strtol(js, 0, 10);
    else {
        js = getenv("SLURM_JOB_ID");
        if (js)
            jid = (int)strtol(js, 0, 10);
    }
    if (jid > 0)
        jobid = jid;
    jobID = jobid;

    // set up component id from hostname, if hostname has a number in it
    int cid = 0;
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        char* idp = strpbrk(hostname, "0123456789");
        if (idp)
            nodeID = cid = (int)strtol(idp, 0, 10);
    }
    // try to get number of nodes from job scheduler
    char* ns = getenv("PBS_NUM_NODES");
    if (ns)
        numNodes = (int)strtol(ns, 0, 10);
    else {
        ns = getenv("SLURM_JOB_NUM_NODES");
        if (ns)
            numNodes = (int)strtol(ns, 0, 10);
    }
    // try to get number of processes (ranks) from job scheduler
    char* ps = getenv("PBS_NP");
    if (ps)
        numProcs = (int)strtol(ps, 0, 10);
    else {
        ps = getenv("SLURM_NTASKS");
        if (ps)
            numProcs = (int)strtol(ps, 0, 10);
    }
    // get user id
    userID = getuid();

    // initialize base metrics
    baseCount = 0;
    baseMetrics[baseCount].name = "timemsec";
    baseMetrics[baseCount].type = UINT64;
    baseMetrics[baseCount++].v.i64val = 0;
    baseMetrics[baseCount].name = "component";
    baseMetrics[baseCount].type = UINT32;
    baseMetrics[baseCount++].v.i32val = cid;
    baseMetrics[baseCount].name = "appid";
    baseMetrics[baseCount].type = UINT32;
    baseMetrics[baseCount++].v.i32val = appid;
    baseMetrics[baseCount].name = "jobid";
    baseMetrics[baseCount].type = UINT32;
    baseMetrics[baseCount++].v.i32val = jobid;
    baseMetrics[baseCount].name = "rank";
    baseMetrics[baseCount].type = UINT32;
    baseMetrics[baseCount++].v.i32val = rank;
    baseMetrics[baseCount].name = "pid";
    baseMetrics[baseCount].type = UINT32;
    baseMetrics[baseCount++].v.i32val = getpid();

    // initialize thread metrics
    metricCount = 0;
    threadMetrics[0][metricCount].name = strdup("threadID");
    threadMetrics[0][metricCount].v.u64val = 0;
    threadMetrics[0][metricCount++].type = UINT64;
    // declare metrics for hbeat count and time
    for (i = 1; i <= numHeartbeats; i++) {
        char buffer[48];
        snprintf(buffer, sizeof(buffer) - 2, "hbcount%d", i);
        // initialize for thread index 0, we'll use others later?
        // when writing CSV header, we only need one copy, so only
        // put names in thread index 0
        threadMetrics[0][metricCount].name = strdup(buffer);
        threadMetrics[0][metricCount].v.u64val = 0;
        threadMetrics[0][metricCount++].type = UINT64;
        snprintf(buffer, sizeof(buffer) - 2, "hbduration%d", i);
        threadMetrics[0][metricCount].name = strdup(buffer);
        threadMetrics[0][metricCount].v.dpval = 0.0;
        threadMetrics[0][metricCount++].type = DOUBLE;
    }
    appekgStatus = APPEKG_STAT_OK;
    // decide and initialize output mode
    char* es = getenv("APPEKG_OUTPUT_MODE");
    if (!es || !strcmp("CSV",es)) {
        ekgOutputMode = DO_CSV_FILES;
        // too early, let CSV code do it on demand
        // -- missing HB names for JSON header at this point
        // initializeCSVOutput();
#ifdef INCLUDE_LDMS
    } else if (!strcmp("LDMS",es)) {
        ekgOutputMode = DO_LDMS_STREAMS;
        initializeLDMSOutput();
#endif
#ifdef INCLUDE_SQLITE
    } else if (!strcmp("SQLITE",es)) {
        ekgOutputMode = DO_SQLITE;
        initializeSQLiteOutput(); 
#endif
    }
    // start up sampling thread
    doSampling = 1;
    pthread_mutex_unlock(&hblock);
    pthread_create(&samplingThreadId, 0, performSampling, 0);
    return 0;
}

// forward declaration for compiler
static void finalizeHeartbeatData(void* v);

/**
* \brief Finalize AppEKG
* \return 0 on success. negative value is returned on failure.
**/
void ekgFinalize(void)
{
    //int i;
    /* if never initialized, don't clean up */
    if (appekgStatus != APPEKG_STAT_OK)
        return;
    pthread_mutex_lock(&hblock);
    if (appekgStatus != APPEKG_STAT_OK) {
        pthread_mutex_unlock(&hblock);
        return;
    }
    doSampling = 0;
    pthread_mutex_unlock(&hblock);
    // calling finalize doesn't work here because the sampling
    // thread calls it upon thread exit; so we're trying here
    // to just cancel the sampling thread
    //finalizeHeartbeatData((void*)0);
    pthread_cancel(samplingThreadId);
    // but finalize will now happen at some later point, so we 
    // can't just delete everything here.
    /***
    appekgStatus = APPEKG_STAT_DISABLED;
    if (_ekgHBEndFlag) {
        free(_ekgHBEndFlag);
        _ekgHBEndFlag = 0;
    }
    if (_ekgHBCount) {
        free(_ekgHBCount);
        _ekgHBCount = 0;
    }
    if (_ekgActualThreadID) {
        free(_ekgActualThreadID);
        _ekgActualThreadID = 0;
    }
    // metric[0][0] is threadID, then two (count,duration) for each hearbeat
    for (i=0; i <= numHeartbeats * 2; i++) {
        if (threadMetrics[0][i].name) {
            free(threadMetrics[0][i].name);
            threadMetrics[0][i].name = 0;
        }
    }
    ***/
    return;
}

/**
* \brief Stop AppEKG data collection temporarily (TODO)
**/
void ekgDisable(void)
{
    // TODO
}

/**
* \brief Start or restart AppEKG data collection (TODO)
**/
void ekgEnable(void)
{
    // TODO
}

/**
* \brief Begin a heartbeat
*
* Creates a timestamp to begin this heartbeat; although it does
* not do any locking, it should be thread safe because each thread
* has its own data, and thread hash collisions are handled before
* setting data.
*
* \param id is the heartbeat ID
*/
void ekgBeginHeartbeat(unsigned int hbId)
{
    if (hbId <= 0 || hbId > numHeartbeats)
        return;
    if (appekgStatus != APPEKG_STAT_OK)
        return;
    unsigned int thId = THREAD_ID;
    unsigned int realId = THREAD_ID_FULL;
#ifdef DEBUG
        fprintf(stderr,"ekgBHF: %u tid %u rid %u stid %u\n", hbId, thId, realId,
                _ekgActualThreadID[thId]);
#endif
    if (_ekgActualThreadID[thId] == 0)
        _ekgActualThreadID[thId] = realId;
    else if (_ekgActualThreadID[thId] != realId)
        return; // collision and we didn't win, so leave
    struct timespec start;
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
        if (allowStderr)
            perror("AppEKG: clock gettime error");
        return;
    }
    // subtract off program start time seconds (skip nanosec)
    start.tv_sec -= programStartTime.tv_sec;
    // changed to be microseconds; nanoseconds might overflow an int
    // TODO: still think about this, maybe microseconds will overflow, too
    beginHBTime[thId][hbId] =
          ((1000000 * start.tv_sec) + (start.tv_nsec / 1000));
    return;
}

/**
* \brief End a heartbeat
* 
* This function timestamps the end of the current heartbeat, 
* and then updates the calculation of the count and durations
* of the occurrences of this heartbeat.
* TODO: make this simpler!
*
* \param id is the heartbeat ID
**/
void ekgEndHeartbeat(unsigned int hbId)
{
    double duration, avg;
    if (hbId <= 0 || hbId > numHeartbeats)
        return;
    if (appekgStatus != APPEKG_STAT_OK)
        return;
    struct timespec endTime;
    unsigned int thId = THREAD_ID;
#ifdef DEBUG
        fprintf(stderr,"ekgEHF: %u tid %u rid %u stid %u\n", hbId, thId,
                THREAD_ID_FULL, _ekgActualThreadID[thId]);
#endif
    if (_ekgActualThreadID[thId] != THREAD_ID_FULL)
        return;
    if (clock_gettime(CLOCK_REALTIME, &endTime) == -1) {
        if (allowStderr)
            perror("AppEKG: clock gettime error");
        return;
    }
    // subtract off program start time seconds (skip nanosec)
    endTime.tv_sec -= programStartTime.tv_sec;
    // convert secs to microsecond, see comment in beginHB
    unsigned long endHBTime =
          ((1000000 * endTime.tv_sec) + (endTime.tv_nsec / 1000));
    // calculate duration
    duration = (endHBTime - beginHBTime[thId][hbId]);
    beginHBTime[thId][hbId] = 0;
    // lock count and duration update from other threads
#ifdef EKG_USE_OPENMP
#pragma omp critical
{
#else
    pthread_mutex_lock(&hblock);
#endif
    hbId = (hbId - 1) * 2 + 1; // map to thread data array
    if (threadMetrics[thId][hbId].v.u64val > 0) {
        // too much arithmetic for high-intensity heartbeats, this
        // could be simplified if we just kept the duration sum
        avg = ((threadMetrics[thId][hbId + 1].v.dpval *
                      (threadMetrics[thId][hbId].v.u64val) +
                duration) /
               (threadMetrics[thId][hbId].v.u64val + 1));
        threadMetrics[thId][hbId + 1].v.dpval = avg;
        threadMetrics[thId][hbId].v.u64val++;
    } else {
        threadMetrics[thId][hbId + 1].v.dpval = duration;
        threadMetrics[thId][hbId].v.u64val++;
    }
#ifdef EKG_USE_OPENMP
}
#else
    pthread_mutex_unlock(&hblock);
#endif
    return;
}

/**
* \brief Provide a name for a heartbeat
*
* Give AppEKG a name for a heartbeat; AppEKG will store its own copy.
*
* \param id is the heartbeat ID
* \param name is the string name
* \return 0 on success, -1 on failure
**/
int ekgNameHeartbeat(unsigned int id, char* name)
{
    if (id < 1 || id > numHeartbeats)
        return -1;
    if (hbNames[id] != 0)
        free(hbNames[id]);
    hbNames[id] = strdup(name);
    return 0;
}

/**
* \brief Find ID of heartbeat from name
*
* Find ID of the given named heartbeat
*
* \param name is the string name
* \return ID value on success, 0 on failure
**/
unsigned int ekgIdOfHeartbeat(char* name)
{
    int i;
    // valid IDs start at 1, index 0 is not used
    for (i = 1; i <= numHeartbeats; i++) {
        if (hbNames[i] == 0)
            continue;
        if (!strcmp(hbNames[i], name))
            return i;
    }
    return 0;
}

/**
* \brief Find name of heartbeat from ID
*
* Ask for stored name of heartbeat; returns NULL if none, ptr to 
* new copy of string if found.
*
* \param id is the heartbeat ID
* \return new allocated copy of name on success, 0 on failure
**/
char* ekgNameOfHeartbeat(unsigned int id)
{
    if (id < 1 || id > numHeartbeats || hbNames[id] == 0)
        return 0;
    return strdup(hbNames[id]);
}

/**
* \brief Write Metadata out in JSON format
*
* TODO: add more fields, possibly number of active threads, sampling interval,
*       more? Get some MPI env variables, number of processes, etc.
* page with environment variables: https://hpcc.umd.edu/hpcc/help/slurmenv.html
* number of nodes: SLURM_JOB_NUM_NODES, PBS_NUM_NODES
* number of procs: SLURM_NTASKS, PBS_NP
* end time: ?? need another function to add to JSON data
**/
static char* metadataToJSON()
{
    static char tdatastr[2048]; // static to enable return; not thread safe
    int i, tstrlen = 0;
    tstrlen +=
          snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1, "{\n");
    // i starts at 1 to skip timestamp, but include component, appid, jobid,
    // rank, and pid
    for (i = 1; i < baseCount; i++) {
        if (i > 1)
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, ",\n");
        switch (baseMetrics[i].type) {
        case UINT8:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%u",
                                baseMetrics[i].name, baseMetrics[i].v.u8val);
            break;
        case UINT16:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%u",
                                baseMetrics[i].name, baseMetrics[i].v.u16val);
            break;
        case UINT32:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%u",
                                baseMetrics[i].name, baseMetrics[i].v.u32val);
            break;
        case UINT64:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%lu",
                                baseMetrics[i].name, baseMetrics[i].v.u64val);
            break;
        case INT8:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%d",
                                baseMetrics[i].name, baseMetrics[i].v.i8val);
            break;
        case INT16:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%d",
                                baseMetrics[i].name, baseMetrics[i].v.i16val);
            break;
        case INT32:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%d",
                                baseMetrics[i].name, baseMetrics[i].v.i32val);
            break;
        case INT64:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%ld",
                                baseMetrics[i].name, baseMetrics[i].v.i64val);
            break;
        case FLOAT:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%g",
                                baseMetrics[i].name, baseMetrics[i].v.fpval);
            break;
        case DOUBLE:
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, "\"%s\":%g",
                                baseMetrics[i].name, baseMetrics[i].v.dpval);
            break;
        //case STRING: fprintf(TODO,"%s",baseMetrics[i].v.strval); break;
        default:
            tstrlen +=
                  snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                           "\"%s\":\"unknown\"", baseMetrics[i].name);
            break;
        }
    }
    // Output some extra information
    char hostname[48];
    struct utsname unameInfo;
    tstrlen +=
          snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                   ",\n\"starttime\":%u", (unsigned)programStartTime.tv_sec);
    gethostname(hostname, sizeof(hostname) - 1);
    tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                        ",\n\"hostname\":\"%s\"", hostname);
    uname(&unameInfo);
    tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                        ",\n\"osname\":\"%s\"", unameInfo.sysname);
    // nodename is same as hostname (but hostname may be more reliable)
    //tstrlen += snprintf(tdatastr+tstrlen, sizeof(tdatastr)-tstrlen-1,
    //                    ",\n\"nodename\":\"%s\"", unameInfo.nodename);
    tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                        ",\n\"osrelease\":\"%s\"", unameInfo.release);
    tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                        ",\n\"osversion\":\"%s\"", unameInfo.version);
    tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                        ",\n\"architecture\":\"%s\"", unameInfo.machine);
    tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                        ",\n\"numprocs\":%d", numProcs);
    // heartbeat names are put in an array
    tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                        ",\n\"hbnames\":{");
    for (i = 1; i <= numHeartbeats; i++) {
        if (hbNames[i] != 0) {
            if (i > 1)
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, ",");
            tstrlen +=
                  snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                           "\"%d\":\"%s\"", i, hbNames[i]);
        }
    }
    tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                        "}\n}\n");
    return tdatastr;
}

/**
* \brief Output hearbeat data to CSV file or LDMS stream
*
* This routine writes out a sampled datapoint; it was recently
* refactored to not directly include code for each output data
* format, and needs more work to re-include LDMS streams, and
* other data sinks. TODO: Should allow both (and more) to be 
* compiled in, runtime selected
**/
static void outputHeartbeatData()
{
    struct timeval curtime;
    if (appekgStatus != APPEKG_STAT_OK)
        return;
    // set current sample's timestamp
    gettimeofday(&curtime, 0);
    curtime.tv_sec -= programStartTime.tv_sec;
    baseMetrics[0].v.i64val =
          (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);
    //baseMetrics[0].v.i64val = curtime.tv_sec;
    //baseMetrics[1].v.i64val = curtime.tv_usec;
    if (ekgOutputMode == DO_CSV_FILES)
        outputCSVData();
#ifdef INCLUDE_LDMS
    else if (ekgOutputMode == DO_LDMS_STREAMS)
        outputLDMSStreamsData();
#endif
#ifdef INCLUDE_SQLITE
    else if (ekgOutputMode == DO_SQLITE)
        outputSQLiteData();
#endif
    // TODO Other choices here?
}

/**
* \brief Finish AppEKG data collection
*
* This writes one last datapoint out (perhaps it should not?)
* and closes the output mechanism.
*
* \param arg is not used (but required by pthreads)
**/
static void finalizeHeartbeatData(void* arg)
{
    if (appekgStatus != APPEKG_STAT_OK)
        return;
#ifdef EKG_DO_UNFINISHED_HBS
    // JEC: We check for any unfinished heartbeats and then
    // end them forcefully. It seems like some threads may leave 
    // unfinished heartbeats; compile this in by #defining var
    struct timespec endTime;
    unsigned int tid, i;
    double duration, avg;
    if (clock_gettime(CLOCK_REALTIME, &endTime) == -1) {
        if (allowStderr)
            perror("AppEKG: clock gettime error");
        return;
    }
    // subtract off program start time seconds (skip nanosec)
    endTime.tv_sec -= programStartTime.tv_sec;
    // convert secs to microsecond, see comment in beginHB
    unsigned long endHBTime =
          ((1000000 * endTime.tv_sec) + (endTime.tv_nsec / 1000));

    for (tid = 0; tid < EKG_MAX_THREADS; tid++) {
        //fprintf(stderr,"Thread %d\n",tid);
        // new: if no thread is registered in this slot, continue,
        // else create and print a data record regardless of whether
        // it is all zeroes or not
        if (_ekgActualThreadID[tid] == 0)
            continue;
        for (i = 1; i <= numHeartbeats; i++) {
            if (beginHBTime[tid][i] > 0) {
                unsigned int hbid = (i-1)*2+1;
                duration = (endHBTime - beginHBTime[tid][i]);
                if (threadMetrics[tid][hbid].v.u64val > 0) {
                    // too much arithmetic for high-intensity heartbeats, this
                    // could be simplified if we just kept the duration sum
                    avg = ((threadMetrics[tid][hbid + 1].v.dpval *
                                  (threadMetrics[tid][hbid].v.u64val) +
                            duration) /
                           (threadMetrics[tid][hbid].v.u64val + 1));
                    threadMetrics[tid][hbid + 1].v.dpval = avg;
                    threadMetrics[tid][hbid].v.u64val++;
                } else {
                    threadMetrics[tid][hbid + 1].v.dpval = duration;
                    threadMetrics[tid][hbid].v.u64val++;
                }
            }
        }
    }
    // JEC END checking for unfinished heartbeats TODO: move to CSV source
#endif
    outputHeartbeatData();
    // finalize output mode
    if (ekgOutputMode == DO_CSV_FILES) {
        finalizeCSVOutput();
#ifdef INCLUDE_LDMS
    } else if (ekgOutputMode == DO_LDMS_STREAMS) {
        finalizeLDMSOutput();
#endif
#ifdef INCLUDE_SQLITE
    } else if (ekgOutputMode == DO_SQLITE) {
        finalizeSQLiteOutput();
#endif
    }
    appekgStatus = APPEKG_STAT_DISABLED;
}

/**
* \brief Main routine for data-collecting thread
*
* This routine is invoked as the beginning of a pthread-created
* thread, and runs for the duration of the application. It samples
* the collected metric data at the desired sampling rate, and invokes
* the output function.
*
* \param arg is not used
* \return always returns 0
**/
static void* performSampling(void* arg)
{
    pthread_detach(pthread_self());
    pthread_cleanup_push((&finalizeHeartbeatData), ((void*)0));
    while (doSampling) {
        sleep(samplingInterval);
        outputHeartbeatData();
    }
    pthread_cleanup_pop(1);
    return 0;
}

