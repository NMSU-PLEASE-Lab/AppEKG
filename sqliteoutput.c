
//
// Skip entire file if not including SQLite support
//
#ifdef INCLUDE_SQLITE

#include <unistd.h>
#include <sqlite3.h>

static sqlite3* dbHandle;

// forward prototypes
static int createSQLiteDB(char* dbName);

static void initializeSQLiteOutput()
{
    char* dbName;
    int stat;
    char* errorMsg;
    char queryString[2048];
    dbName = getenv("APPEKG_SQLITE_DBNAME");
    if (!dbName) {
       dbName = SQLITE_DBNAME;
    }
    // check if db exists, if not create it
    stat = access(dbName, R_OK|W_OK);
    if (stat) {
        if (allowStderr)
            fprintf(stderr, "AppEKG: no sqlite db...creating\n");
        stat = createSQLiteDB(dbName);
    } else // open existing db
        stat = sqlite3_open(dbName, &dbHandle);
    if (stat) {
        if (allowStderr)
            fprintf(stderr, "AppEKG: cannot open sqlite db...disabling\n");
        appekgStatus = APPEKG_STAT_DISABLED;
        sqlite3_close(dbHandle);
        dbHandle = NULL;
        return;
    }
    // must check/insert rows for application, job, and heartbeats
    sprintf(queryString, "INSERT OR IGNORE INTO Application VALUES(%d,\"%s\",\"%s\");",
            applicationID, "UnknownName", "NoVersion");
    stat = sqlite3_exec(dbHandle, queryString, 0, 0, &errorMsg);
    if (stat && allowStderr) {
        fprintf(stderr, "AppEKG: cannot insert into app table (%s)\n",
                errorMsg);
        sqlite3_free(errorMsg);
    }
    // add job (must update end time later)
    sprintf(queryString, "INSERT OR IGNORE INTO Job VALUES(%d,%d,%ld,%ld,%d,%d,%d);",
            jobID, userID, programStartTime.tv_sec, programStartTime.tv_sec, 
            numNodes, numProcs, applicationID);
    stat = sqlite3_exec(dbHandle, queryString, 0, 0, &errorMsg);
    if (stat && allowStderr) {
        fprintf(stderr, "AppEKG: cannot insert into job table (%s)\n",
                errorMsg);
        sqlite3_free(errorMsg);
    }
    // add heartbeats
    for (int i=1; i <= numHeartbeats; i++) {
        sprintf(queryString, "INSERT OR IGNORE INTO Heartbeat "
                             "VALUES(NULL,%d,%d,\"%s\");",
                applicationID, i, hbNames[i]);
        stat = sqlite3_exec(dbHandle, queryString, 0, 0, &errorMsg);
        if (stat && allowStderr) {
            fprintf(stderr, "AppEKG: cannot insert into heartbeat table (%s)\n",
                    errorMsg);
            sqlite3_free(errorMsg);
        }
    }
    return;
}

static int processAdded = 0;

static void outputSQLiteData()
{
    char* errorMsg = 0;
    int stat;
    char queryString[2048];
    long sampleID;
    //
    // below is copied from CSV output, needs formatted into SQL query
    //
    int i;
    unsigned int tid;
    if (appekgStatus != APPEKG_STAT_OK)
        return;
    //fprintf(csvFH,"CSV Output\n");
    pthread_mutex_lock(&hblock);
    // add process record if needed
    if (!processAdded) {
        sprintf(queryString,
                "INSERT OR IGNORE INTO Process VALUES(NULL,%d,%d,%d,%d,%d);",
                getpid(), nodeID, jobID, rankID, 0); // TODO: NUMTHREADS!
        stat = sqlite3_exec(dbHandle, queryString, 0, 0, &errorMsg);
        if (stat && allowStderr) {
            fprintf(stderr, "AppEKG: cannot insert into process table (%s)\n",
                    errorMsg);
            sqlite3_free(errorMsg);
        }
        processAdded = 1;
    }
    if (appekgStatus != APPEKG_STAT_OK) {
        pthread_mutex_unlock(&hblock);
        return;
    }
    int logicalThID = 0;
    for (tid = 0; tid < EKG_MAX_THREADS; tid++) {
        //fprintf(csvFH,"Thread %d\n",tid);
        // new: if no thread is registered in this slot, continue,
        // else create and print a data record regardless of whether
        // it is all zeroes or not
        if (_ekgActualThreadID[tid] == 0)
            continue;
        logicalThID++;
        // add EKGSample database row (don't use _ekgActualThreadID[tid]
        sprintf(queryString, "INSERT INTO EKGSample VALUES(NULL,%ld,%d,%d);",
            baseMetrics[0].v.i64val, logicalThID, getpid());
        stat = sqlite3_exec(dbHandle, queryString, 0, 0, &errorMsg);
        if (stat && allowStderr) {
            fprintf(stderr, "AppEKG: cannot insert into ekgsample table (%s)\n",
                    errorMsg);
            sqlite3_free(errorMsg);
        }
        // get sample key
        sampleID = sqlite3_last_insert_rowid(dbHandle);
        // now loop through hearbeats and add to HBSample table
        for (i = 0; i <= numHeartbeats; i++) {
            sprintf(queryString, "INSERT INTO HBSample VALUES(NULL,%ld,%g,%d,%ld);",
                    threadMetrics[tid][i*2+1].v.u64val, 
                    threadMetrics[tid][i*2+2].v.dpval, i+1, sampleID);
            stat = sqlite3_exec(dbHandle, queryString, 0, 0, &errorMsg);
            if (stat && allowStderr) {
                fprintf(stderr, "AppEKG: cannot insert into hbsample table (%s)\n",
                        errorMsg);
                sqlite3_free(errorMsg);
            }
            // Zero out all heartbeat data
            threadMetrics[tid][i*2+1].v.dpval = 0.0;
            threadMetrics[tid][i*2+2].v.dpval = 0.0;
        }
        //if (rc != SQLITE_OK) {
        //    if (allowStderr)
        //        fprintf(stderr, "SQL error: %s\n", errMsg);
        //    sqlite3_free(errMsg);
        //}
    }                              // end outer for
    pthread_mutex_unlock(&hblock); // unlock after all HB data
    //fflush(csvFH);
    // 
    // end copy from CSV output
    //
}

static void finalizeSQLiteOutput()
{
    sqlite3_close(dbHandle);
    dbHandle = NULL;
}

static char* createTablesString = 
"CREATE TABLE IF NOT EXISTS Application (\n\
   id INTEGER PRIMARY KEY NOT NULL,\n\
   name TEXT,\n\
   version TEXT\n\
);\n\
CREATE TABLE IF NOT EXISTS Heartbeat (\n\
   id INTEGER PRIMARY KEY AUTOINCREMENT,\n\
   app_id INTEGER,\n\
   apphb_id INTEGER,\n\
   name TEXT,\n\
   FOREIGN KEY(app_id) REFERENCES Application\n\
);\n\
CREATE TABLE IF NOT EXISTS Job (\n\
   id INTEGER,\n\
   user INTEGER,\n\
   begin_time INTEGER,\n\
   end_time INTEGER,\n\
   num_nodes INTEGER,\n\
   num_processes INTEGER,\n\
   app_id INTEGER,\n\
   PRIMARY KEY(id),\n\
   FOREIGN KEY(app_id) REFERENCES Application\n\
);\n\
CREATE TABLE IF NOT EXISTS Process (\n\
   id INTEGER PRIMARY KEY AUTOINCREMENT,\n\
   pid INTEGER,\n\
   node_id INTEGER,\n\
   job_id INTEGER,\n\
   rank INTEGER,\n\
   num_threads INTEGER,\n\
   FOREIGN KEY(job_id) REFERENCES Job\n\
);\n\
CREATE TABLE IF NOT EXISTS EKGSample (\n\
   id INTEGER PRIMARY KEY AUTOINCREMENT,\n\
   time_msec INTEGER,\n\
   thread_id INTEGER,\n\
   process_id INTEGER,\n\
   FOREIGN KEY(process_id) REFERENCES Process\n\
);\n\
CREATE TABLE IF NOT EXISTS HBSample (\n\
   id INTEGER PRIMARY KEY AUTOINCREMENT,\n\
   num_heartbeats INTEGER,\n\
   avg_duration REAL,\n\
   hb_id INTEGER,\n\
   sample_id INTEGER,\n\
   FOREIGN KEY(sample_id) REFERENCES EKGSample,\n\
   FOREIGN KEY(hb_id) REFERENCES Heartbeat\n\
);";

//
// create a new sqlite db file and create tables in it
//
static int createSQLiteDB(char* dbName)
{
    char* errorMsg;
    int stat;
    stat = sqlite3_open(dbName, &dbHandle); // should create and open it
    if (stat) {
        if (allowStderr)
            fprintf(stderr, "AppEKG: cannot open+create sqlite db\n");
        return stat;
    }
    stat = sqlite3_exec(dbHandle, createTablesString, 0, 0, &errorMsg);
    if (stat) {
        if (allowStderr)
            fprintf(stderr, "AppEKG: cannot create tables in sqlite db\n");
    }
    return stat;
}

#endif

