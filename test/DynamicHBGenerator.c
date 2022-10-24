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

#include "appekg.h"

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
    EKG_INITIALIZE(3, 1, 101, 42, 13, 1);
    //EKG_NAME_HEARTBEAT(1,"hb1");
    //EKG_NAME_HEARTBEAT(2,"hb2");
    //EKG_NAME_HEARTBEAT(3,"hb3");

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
    EKG_FINALIZE();

    return 0;
}

void logHeartbeats(int heartbeatID, int numberOfHBForEachCycle, int durationOfEachHB, int intervalBetweenHBs){
    
    for(int k=0;k<numberOfHBForEachCycle;k++){
        // start HB tracking
        EKG_BEGIN_HEARTBEAT(heartbeatID, 1);
        
        usleep(TO_MILLISECOND_FACTOR * durationOfEachHB);
        //msleep(durationOfEachHB);

        // end HB tracking
        EKG_END_HEARTBEAT(heartbeatID);

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


