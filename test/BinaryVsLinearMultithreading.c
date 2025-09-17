//
// AppEKG example program that does binary, linear, and linear recursive
// search over an array of values.
//
// set DELAY to be the number of microseconds to delay in each search step
// - can be 0
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "appekg.h"

#define ARRAYSIZE 50000
#define DELAY 200

int binarySearch(int arr[], int l, int r, int x);
int linearSearch(int arr[], int n, int x);
int linearSearchRecursive(int arr[], int n, int x);

void *binarySearchThread(void *searchValue);
void *linearSearchThread(void *searchValue);
void *linearSearchRecursiveThread(void *searchValue);

int arr[ARRAYSIZE];

int main(void)
{
    // initializing AppEKG library
    EKG_INITIALIZE(3, 1, 104, 54, 14, 1);
    EKG_NAME_HEARTBEAT(1,"BinarySearchMT");
    EKG_NAME_HEARTBEAT(2,"LinearSearchMT");
    EKG_NAME_HEARTBEAT(3,"LinearRecSearchMT");
   // declare array and initialize values
   int n = ARRAYSIZE;
   for (int i=0; i < n; i++) {
       arr[i] = i;
   }
   // select random x value for search operation
   srand(time(0));
   int x = rand() % (n*2); // fail about 50% of time
    // create and launch pthreads for searching
    pthread_t threads[3];
    int rc[3];

    rc[0] = pthread_create(&threads[0], NULL, binarySearchThread, &x);
    if (rc[0]) {
        printf("Error:unable to create thread\n");
        exit(-1);
    }
    rc[1] = pthread_create(&threads[1], NULL, linearSearchThread, &x);
    if (rc[1]) {
        printf("Error:unable to create thread\n");
        exit(-1);
    }
    rc[2] = pthread_create(&threads[2], NULL, linearSearchRecursiveThread, &x);
    if (rc[2]) {
        printf("Error:unable to create thread\n");
        exit(-1);
    }
    /* block until all threads complete */
    for (int i = 0; i < 3; ++i) {
        pthread_join(threads[i], NULL);
    }
    // finalizing/closing AppEKG library
    EKG_FINALIZE();
    return 0;
}

void *binarySearchThread(void *searchValue)
{
    int x;
    x = *((int*)searchValue);
    // performing binary search operation and storing the result
    int resultBinary = binarySearch(arr, 0, ARRAYSIZE - 1, x);
    if(resultBinary == -1) 
        printf("Binary Search: Element %d is not present in array\n", x); 
    else 
        printf("Binary Search: Element %d is present at index %d\n", x, resultBinary);
    pthread_exit(NULL);
}

int binarySearch(int arr[], int l, int r, int x) {
    // start HB tracking
    EKG_BEGIN_HEARTBEAT(1, 1);
    if (DELAY > 0) usleep(DELAY);
    if (r >= l) {
        int mid = l + (r - l) / 2;
        // if the element is present at the middle itself
        if (arr[mid] == x) {
            // end HB tracking
            EKG_END_HEARTBEAT(1);
            return mid;
        }
        // if element is smaller than mid, then it can only be present in left subarray
        if (arr[mid] > x) {
            // end HB tracking
            EKG_END_HEARTBEAT(1);
            return binarySearch(arr, l, mid - 1, x);
        }
        // end HB tracking
        EKG_END_HEARTBEAT(1);
        // else the element can only be present in right subarray
        return binarySearch(arr, mid + 1, r, x);
    }
    EKG_END_HEARTBEAT(1);
    // We reach here when element is not present in array
    return -1;
}

void *linearSearchThread(void *searchValue)
{
    int x;
    x = *((int*)searchValue);
    // performing linear search operation and storing the result
    int resultLinear = linearSearch(arr, ARRAYSIZE, x);
    if(resultLinear == -1) 
        printf("Linear Search: Element %d is not present in array\n", x); 
    else 
        printf("Linear Search: Element %d is present at index %d\n", x, resultLinear);
    pthread_exit(NULL);
}

int linearSearch(int arr[], int n, int x)
{
    for (int i=0; i < n; i++) {
        // start HB tracking
        EKG_BEGIN_HEARTBEAT(2, 1);
        if (DELAY > 0) usleep(DELAY);
        if (arr[i] == x) {
            // end HB tracking
            EKG_END_HEARTBEAT(2);
            return i;
        }
        // end HB tracking
        EKG_END_HEARTBEAT(2);
    }
    return -1;
}

void *linearSearchRecursiveThread(void *searchValue)
{
    int x;
    x = *((int*)searchValue);
    // performing linear search operation recursively and storing the result
    int resultLinearRecursive = linearSearchRecursive(arr, ARRAYSIZE, x);
    if(resultLinearRecursive == -1) 
        printf("Linear Search (Recursive): Element %d is not present in array\n", x); 
    else 
        printf("Linear Search (Recursive): Element %d is present at index %d\n", x,
               resultLinearRecursive);
    pthread_exit(NULL);
}

// Recursive function to search x in arr[]
int linearSearchRecursive(int arr[], int size, int x)
{
    // start HB tracking
    EKG_BEGIN_HEARTBEAT(3, 1);
    if (DELAY > 0) usleep(DELAY);
    size--;
    if (size >= 0) {
        if (arr[size] == x) {
            // end HB tracking
            EKG_END_HEARTBEAT(3);
            return size;
        } else {
            EKG_END_HEARTBEAT(3);
            return linearSearchRecursive(arr, size, x);
        }
    } else {
        EKG_END_HEARTBEAT(3);
        return -1;
    }
    // should not be possible to get here
    EKG_END_HEARTBEAT(3);
    return 0;
}


