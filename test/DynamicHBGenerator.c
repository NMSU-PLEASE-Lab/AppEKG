/*

This program generates HBs dynamically by taking input from command line.

The following values can be given as input:
* number of HB 
* number of Cycle 
* duration of dach HB (ms) 
* interval between HBs (ms) 
* number of HB for each cycle

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
#define LDMSDEF
#include </Users/sudippodder/Documents/RA_Works/appheartbeats/ldmsa.h>
#endif

void logHeartbeats(int heartbeatID, int numberOfHBForEachCycle, int durationOfEachHB, int intervalBetweenHBs);
int msleep(long tms);

int TO_MILLISECOND_FACTOR = 1000;

int main(int argc, char *argv[]) {

    if(argc != 6) {
        printf("\n");
        printf("Five integer arguments expected in the following order:\n\n");
        printf("numberOfHB numberOfCycle durationOfEachHB(ms) intervalBetweenHBs(ms) numberOfHBForEachCycle:\n\n");
        return 0;
    }

    // initializing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        int ldms_stat;
        ldms_stat = appekg_initialize(1,0,0,0,0);
        if (ldms_stat == -1)
            fprintf(stderr,"LDMS error initializing: %d\n",ldms_stat);
    #endif

    // capture data in integer form from command line input
    int numberOfHB = atoi(argv[1]);
    int numberOfCycle = atoi(argv[2]);
    int durationOfEachHB = atoi(argv[3]);
    int intervalBetweenHBs = atoi(argv[4]);
    int numberOfHBForEachCycle = atoi(argv[5]);

    //printf("Number of HB: %d\n", numberOfHB);
    //printf("Number of Cycle: %d\n", numberOfCycle);
    //printf("Duration of Each HB: %d\n", durationOfEachHB);
    //printf("Interval between HBs: %d\n", intervalBetweenHBs);
    //printf("Number of HB for Each Cycle: %d\n", numberOfHBForEachCycle);

    // this loop controls the number of cycle
    for(int i=0;i<numberOfCycle;i++){

        // this loop control the number of HBs
        for(int j=0;j<numberOfHB;j++){

            // starts a HB ID from 1
            int heartbeatID = j+1;

            // starts capturing HBs
            logHeartbeats(heartbeatID, numberOfHBForEachCycle, durationOfEachHB, intervalBetweenHBs);

            //interval between each HB
            usleep(TO_MILLISECOND_FACTOR * durationOfEachHB);
            //msleep(intervalBetweenHBs);
        }
    }

    // finalizing/closing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekg_finalize();
    #endif

    return 0;
}

void logHeartbeats(int heartbeatID, int numberOfHBForEachCycle, int durationOfEachHB, int intervalBetweenHBs){
    
    for(int k=0;k<numberOfHBForEachCycle;k++){
        // start HB tracking
        #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
            appekg_begin_heartbeat(heartbeatID);
        #endif
        
        usleep(TO_MILLISECOND_FACTOR * durationOfEachHB);
        //msleep(durationOfEachHB);

        // end HB tracking
        #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
            appekg_end_heartbeat(heartbeatID);
        #endif

    }

}

// usleep is deprecated, need to use 'nanosleep' instead [need to understand how it works] 
int msleep(long tms){
    struct timespec ts;
    int ret;

    if (tms < 0){
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = tms / 1000;
    ts.tv_nsec = (tms % 1000) * 1000000;

    do {
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

    return ret;
}


