#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<time.h>

#include <pthread.h>

#if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
#define LDMSDEF
#include </Users/sudippodder/Documents/RA_Works/appheartbeats/ldmsa.h>
#endif

int binarySearch(int arr[], int l, int r, int x);
int linearSearch(int arr[], int n, int x);
int linearSearchRecursive(int arr[], int n, int x);

void *binarySearchThread(void *searchValue);
void *linearSearchThread(void *searchValue);
void *linearSearchRecursiveThread(void *searchValue);

int main(void) {
    // initializing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        int ldms_stat;
        ldms_stat = appekg_initialize(1,0,0,0,0);
        if (ldms_stat == -1)
            fprintf(stderr,"LDMS error initializing: %d\n",ldms_stat);
    #endif

    // finding random x value for search operation
    srand(time(0));
    int x = rand() % 32767;
    //int x = 32000;

    pthread_t threads[3];
    int rc[3];

    rc[0] = pthread_create(&threads[0], NULL, binarySearchThread, (void *)x);
    if (rc[0]) {
        printf("Error:unable to create thread\n");
        exit(-1);
    }

    rc[1] = pthread_create(&threads[1], NULL, linearSearchThread, (void *)x);
    if (rc[1]) {
        printf("Error:unable to create thread\n");
        exit(-1);
    }

    rc[2] = pthread_create(&threads[2], NULL, linearSearchRecursiveThread, (void *)x);
    if (rc[2]) {
        printf("Error:unable to create thread\n");
        exit(-1);
    }

    /* block until all threads complete */
    for (int i = 0; i < 3; ++i) {
        pthread_join(threads[i], NULL);
    }

    // finalizing/closing AppEKG library
    #if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
        appekg_finalize();
    #endif

    return 0;
}

void *binarySearchThread(void *searchValue){
    // declaring array size values
    int n = 32767;
    int arr[n];
    for(int i=0;i<n;i++){
        arr[i] = i;
    }

    int x;
    x = (long)searchValue;

    // performing binary search operation and storing the result
    int resultBinary = binarySearch(arr, 0, n - 1, x);

    if(resultBinary == -1) 
        printf("Binary Search: Element %d is not present in array", x); 
    else 
        printf("Binary Search: Element %d is present at index %d", x, resultBinary);

    printf("\n");

    pthread_exit(NULL);
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

void *linearSearchThread(void *searchValue){
    // declaring array size values
    int n = 32767;
    int arr[n];
    for(int i=0;i<n;i++){
        arr[i] = i;
    }

    int x;
    x = (long)searchValue;

    // performing linear search operation and storing the result
    int resultLinear = linearSearch(arr, n, x);

    if(resultLinear == -1) 
        printf("Linear Search: Element %d is not present in array", x); 
    else 
        printf("Linear Search: Element %d is present at index %d", x, resultLinear);

    printf("\n");

    pthread_exit(NULL);
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

void *linearSearchRecursiveThread(void *searchValue){
    // declaring array size values
    int n = 32767;
    int arr[n];
    for(int i=0;i<n;i++){
        arr[i] = i;
    }

    int x;
    x = (long)searchValue;

    // performing linear search operation recursively and storing the result
    int resultLinearRecursive = linearSearchRecursive(arr, n, x);

    if(resultLinearRecursive == -1) 
        printf("Linear Search (Recursive): Element %d is not present in array", x); 
    else 
        printf("Linear Search (Recursive): Element %d is present at index %d", x, resultLinearRecursive);

    printf("\n");

    pthread_exit(NULL);
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
            //#if defined LDMSAPPINFOINCPROF || defined LDMSAPPINFO
                //appekg_end_heartbeat(3);
            //#endif
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



