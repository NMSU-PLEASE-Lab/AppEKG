#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<time.h>

#if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
#define LDMSDEF
#include </Users/sudippodder/Documents/RA_Works/appheartbeats/ldmsa.h>
#endif

int binarySearch(int arr[], int l, int r, int x);
int linearSearch(int arr[], int n, int x);
int linearSearchRecursive(int arr[], int n, int x);

int main(void) {
    // initializing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        int ldms_stat;
        ldms_stat = appekg_initialize(1,0,0,0,0);
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
    int resultBinary = binarySearch(arr, 0, n - 1, x);

    // performing linear search operation and storing the result
    int resultLinear = linearSearch(arr, n, x);

    // performing linear search operation recursively and storing the result
    int resultLinearRecursive = linearSearchRecursive(arr, n, x);

    // printing the result
    if(resultBinary == -1) 
        printf("Binary Search: Element %d is not present in array", x); 
    else 
        printf("Binary Search: Element %d is present at index %d", x, resultBinary);

    printf("\n");

    if(resultLinear == -1) 
        printf("Linear Search: Element %d is not present in array", x); 
    else 
        printf("Linear Search: Element %d is present at index %d", x, resultLinear);

    printf("\n");

    if(resultLinearRecursive == -1) 
        printf("Linear Search (Recursive): Element %d is not present in array", x); 
    else 
        printf("Linear Search (Recursive): Element %d is present at index %d", x, resultLinearRecursive);

    // finalizing/closing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekg_finalize();
    #endif

    return 0;
}

int linearSearch(int arr[], int n, int x){

    for(int i=0;i<n;i++){
        // start HB tracking
        #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
            appekg_begin_heartbeat(2);
        #endif
        if(arr[i]==x){
            // end HB tracking
            #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
                appekg_end_heartbeat(2);
            #endif
            return i;
        }

        // end HB tracking
        #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
            appekg_end_heartbeat(2);
        #endif
    }

    return -1;
}

// Recursive function to search x in arr[]
int linearSearchRecursive(int arr[], int size, int x) {

    // start HB tracking
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekg_begin_heartbeat(3);
    #endif

    int recursive;
  
    size--;
  
    if (size >= 0) {
        if (arr[size] == x){
            // end HB tracking
            #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
                appekg_end_heartbeat(3);
            #endif
            return size;
        }
        else {
            // end HB tracking
            #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
                appekg_end_heartbeat(3);
            #endif
            recursive = linearSearchRecursive(arr, size, x);
        }
    }
    else {
        // end HB tracking
        #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
            appekg_end_heartbeat(3);
        #endif
        return -1;
    }

    // end HB tracking
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekg_end_heartbeat(3);
    #endif
  
    return recursive;
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

