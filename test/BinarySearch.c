#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<time.h>

#if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
#define LDMSDEF
#include </Users/sudippodder/Documents/RA_Works/appheartbeats/ldmsa.h>
#endif

int binarySearch(int arr[], int l, int r, int x);

int main(void) {
    // initializing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        int ldms_stat;
        ldms_stat = appekgapp_initialize(1,0,0,0,0);
        if (ldms_stat == -1)
            fprintf(stderr,"LDMS error initializing: %d\n",ldms_stat);
    #endif

    // declaring array size values
    int n = 32767;
    int arr[n];
    for(int i=0;i<n;i++){
        arr[i] = i;
    }

    //int n = sizeof(arr) / sizeof(arr[0]);

    // finding random x value for search operation
    srand(time(0));
    int x = rand() % n;

    // performing binary search operation and storing the result
    int result = binarySearch(arr, 0, n - 1, x);

    // printing the result
    if(result == -1) 
        printf("Element %d is not present in array", x); 
    else 
        printf("Element %d is present at index %d", x, result);

    // finalizing/closing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekgapp_finalize();
    #endif

    return 0;
}

int binarySearch(int arr[], int l, int r, int x) {
    // start HB tracking
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekg_begin_heartbeat(1);
    #endif

    if (r >= l) {
        int mid = l + (r - l) / 2;
 
        // if the element is present at the middle itself
        if (arr[mid] == x){
            // end HB tracking
            #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
                appekg_end_heartbeat(1);
            #endif

            return mid;
        }
 
        // if element is smaller than mid, then it can only be present in left subarray
        if (arr[mid] > x){
            // end HB tracking
            #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
                appekg_end_heartbeat(1);
            #endif

            return binarySearch(arr, l, mid - 1, x);
        }
            
        // end HB tracking
        #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
            appekg_end_heartbeat(1);
        #endif
        // else the element can only be present in right subarray
        return binarySearch(arr, mid + 1, r, x);
    }
    
    // End HB tracking
        #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
            appekg_end_heartbeat(1);
        #endif

    // We reach here when element is not present in array
    return -1;
}

