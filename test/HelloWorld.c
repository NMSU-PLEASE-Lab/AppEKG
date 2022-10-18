#include <stdio.h>
#include <string.h>

#if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
#define LDMSDEF
#include </Users/sudippodder/Documents/RA_Works/appheartbeats/ldmsa.h>
#endif

void printHelloWorld();

int main(void) {
    // initializing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        int ldms_stat;
        ldms_stat = appekgapp_initialize(1,0,0,0,0);
        if (ldms_stat == -1)
            fprintf(stderr,"LDMS error initializing: %d\n",ldms_stat);
    #endif

    // printing Hello World
    printHelloWorld();

    // printting Hello World multiple times
    /*
    for(int i=0;i<10000;i++){
        printHelloWorld();
    } 
    */

    // finalizing/closing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekgapp_finalize();
    #endif

    return 0;
}

void printHelloWorld(){
    // start HB tracking
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekg_begin_heartbeat(1);
    #endif

    printf ("Hello from your first program!\n");

    // end HB tracking
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekg_end_heartbeat(1);
    #endif
}

