
//
// CSV support is always included, so no conditional compilation directives
// are in this file
//


#define MAX_CSV_STRLEN 1024
static char* metadataToJSON();

static void finalizeCSVOutput()
{
    fclose(csvFH);
    csvFH = 0;
    FILE* jfh = fopen(jsonFilename, "r+");
    if (jfh) {
        fseek(jfh, -3, SEEK_END);
        struct timespec curtime;
        clock_gettime(CLOCK_REALTIME, &curtime);
        fprintf(jfh, ",\n\"endtime\":%u,\n\"duration\":%u\n}\n",
                (unsigned)curtime.tv_sec,
                (unsigned)(curtime.tv_sec - programStartTime.tv_sec));
        fclose(jfh);
        jfh = 0;
    }
}

/**
* \brief Output headers of CSV data files
*
* TODO: this also creates the metadata file (at the end), and so
* this routine probably should be renamed, and/or refactored
**/
static void initializeCSVOutput()
{
    int i;
    char* s;
    char fullFilename[1024], pathFormat[800];
    FILE* mf;
    if (appekgStatus != APPEKG_STAT_OK)
        return;
    // build output data path
    strcpy(fullFilename, ".");
    s = getenv("APPEKG_OUTPUT_PATH");
    if (s) {
        strncpy(pathFormat, s, sizeof(pathFormat));
        pathFormat[sizeof(pathFormat) - 1] = '\0';
        // convert any %s formats into ss, for security
        s = pathFormat;
        while ((s = strstr(s, "%s")) != NULL) {
            *(s + 1) = 'd';
            s++;
        }
        // count # of %s in format; use it if two or less
        s = pathFormat;
        i = 0;
        while ((s = index(s, '%')) != NULL) {
            i++;
            s++;
        }
        if (i <= 2)
            snprintf(fullFilename, sizeof(fullFilename), pathFormat,
                     applicationID, jobID);
        else
            strcpy(fullFilename, pathFormat);
    }
    // copy path prefix back to pathFormat to use twice
    strcpy(pathFormat, fullFilename);
    // try mkdir just in case; this could create the "jobID" directory
    // if it is last and the format specified it
    mkdir(pathFormat, 0770);
    // name each rank's data file with PID
    sprintf(fullFilename, "%s/%s-%d.csv", pathFormat, FILENAME_PREFIX,
            getpid());
    csvFH = fopen(fullFilename, "w");
    if (!csvFH) {
        // try to put data files in current directory
        strcpy(pathFormat, ".");
        sprintf(fullFilename, "%s/%s-%d.csv", pathFormat, FILENAME_PREFIX,
                getpid());
        csvFH = fopen(fullFilename, "w");
    }
    if (!csvFH) {
        if (allowStderr)
            fprintf(stderr, "AppEKG: cannot open data file...disabling");
        appekgStatus = APPEKG_STAT_DISABLED;
        return;
    }
    // write out column headers (CHANGED: only output first one (time)
    // when we finalize this, we can remove the loop
    for (i = 0; i < baseCount; i++) {
        if (i > 0)
            break; // stop after time (was fprintf(csvFH,",");)
        fprintf(csvFH, "%s", baseMetrics[i].name);
    }
    // heartbeats have two metrics each, count and duration, and start at 1
    for (i = 0; i < metricCount; i++) {
        fprintf(csvFH, ",");
        // decision: don't print HB custom names in CSV header
        // - can remove this code when this is a final decision
        //if (i>0 && hbNames[(i-1)/2+1] != 0)
        //   fprintf(csvFH,"%s-%s",threadMetrics[0][i].name,hbNames[(i-1)/2+1]);
        //else
        fprintf(csvFH, "%s", threadMetrics[0][i].name);
    }
    fprintf(csvFH, "\n");
    sprintf(jsonFilename, "%s/%s-%d.json", pathFormat, FILENAME_PREFIX,
            getpid());
    mf = fopen(jsonFilename, "w");
    fputs(metadataToJSON(), mf); // TODO: Too early here, missing HB names
    fclose(mf);
}

/**
* \brief Output heartbeat datapoint to CSV file
*
* This routine collects all heartbeat data per thread into a
* single string of CSV data, then chooses to output it or not,
* depending on whether all threads had 0 data or not. DONE: this
* needs some thinking, we might rather identify the "active" threads
* first, then always output data lines for the active threads, whether
* any heartbeats were non-zero or not.
**/
static void outputCSVData()
{
    int i, tstrlen = 0; //, allzeros;
    unsigned int tid;
    char tdatastr[MAX_CSV_STRLEN];
    if (appekgStatus != APPEKG_STAT_OK)
        return;
    //fprintf(csvFH,"CSV Output\n");
    pthread_mutex_lock(&hblock);
    // if output hasn't started yet, write header data
    if (csvFH == 0)
        initializeCSVOutput();
    // write headers opens the files; if unsuccessful, exit
    if (appekgStatus != APPEKG_STAT_OK) {
        pthread_mutex_unlock(&hblock);
        return;
    }
    for (tid = 0; tid < EKG_MAX_THREADS; tid++) {
        //fprintf(csvFH,"Thread %d\n",tid);
        // new: if no thread is registered in this slot, continue,
        // else create and print a data record regardless of whether
        // it is all zeroes or not
        if (_ekgActualThreadID[tid] == 0)
            continue;
        tstrlen = 0;
        // allzeros = 1;
        // CHANGED: stop after time (first base metric)
        // when we finalize this, we can remove the loop
        for (i = 0; i < baseCount; i++) {
            if (i > 0)
                break; // Stop after first metric
            //tstrlen += snprintf(tdatastr+tstrlen, sizeof(tdatastr)-tstrlen-1,
            //                    ",");
            switch (baseMetrics[i].type) {
            case UINT8:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%u",
                                    baseMetrics[i].v.u8val);
                break;
            case UINT16:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%u",
                                    baseMetrics[i].v.u16val);
                break;
            case UINT32:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%u",
                                    baseMetrics[i].v.u32val);
                break;
            case UINT64:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%lu",
                                    baseMetrics[i].v.u64val);
                break;
            case INT8:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%d",
                                    baseMetrics[i].v.i8val);
                break;
            case INT16:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%d",
                                    baseMetrics[i].v.i16val);
                break;
            case INT32:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%d",
                                    baseMetrics[i].v.i32val);
                break;
            case INT64:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%ld",
                                    baseMetrics[i].v.i64val);
                break;
            case FLOAT:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%g",
                                    baseMetrics[i].v.fpval);
                break;
            case DOUBLE:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%g",
                                    baseMetrics[i].v.dpval);
                break;
            //case STRING: fprintf(csvFH,"%s",baseMetrics[i].v.strval); break;
            default:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "unknown");
                break;
            }
        }
        //allzeros = 1;
        tstrlen += snprintf(tdatastr + tstrlen, sizeof(tdatastr) - tstrlen - 1,
                            ",%d", tid);
        // Currently, heartbeat counts and time start at index 1, not 0,
        // because it makes for easier (faster) math in begin/end heartbeat
        for (i = 1; i < metricCount; i++) {
            tstrlen += snprintf(tdatastr + tstrlen,
                                sizeof(tdatastr) - tstrlen - 1, ",");
            //if (threadMetrics[tid][i].v.dpval != 0.0)
            //   allzeros = 0;
            switch (threadMetrics[0][i].type) {
            case UINT8:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%u",
                                    threadMetrics[tid][i].v.u8val);
                break;
            case UINT16:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%u",
                                    threadMetrics[tid][i].v.u16val);
                break;
            case UINT32:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%u",
                                    threadMetrics[tid][i].v.u32val);
                break;
            case UINT64:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%lu",
                                    threadMetrics[tid][i].v.u64val);
                break;
            case INT8:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%d",
                                    threadMetrics[tid][i].v.i8val);
                break;
            case INT16:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%d",
                                    threadMetrics[tid][i].v.i16val);
                break;
            case INT32:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%d",
                                    threadMetrics[tid][i].v.i32val);
                break;
            case INT64:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%ld",
                                    threadMetrics[tid][i].v.i64val);
                break;
            case FLOAT:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%.3f",
                                    (double)threadMetrics[tid][i].v.fpval);
                break;
            case DOUBLE:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "%.3f",
                                    threadMetrics[tid][i].v.dpval);
                break;
            //case STRING: fprintf(csvFH,"%s",threadMetrics[tid][i].v.strval);
            //  break;
            default:
                tstrlen += snprintf(tdatastr + tstrlen,
                                    sizeof(tdatastr) - tstrlen - 1, "unknown");
                break;
            }
            // Zero out all heartbeat data (but not base metrics)
            threadMetrics[tid][i].v.dpval = 0.0;
        }
        //if (!allzeros)
        fprintf(csvFH, "%s\n", tdatastr);
    }                              // end outer for
    pthread_mutex_unlock(&hblock); // unlock after all HB data
    fflush(csvFH);
}


