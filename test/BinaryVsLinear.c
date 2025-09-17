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
#include "appekg.h"

#define ARRAYSIZE 10000
#define DELAY 200

int binarySearch(int arr[], int l, int r, int x);
int linearSearch(int arr[], int n, int x);
int linearSearchRecursive(int arr[], int n, int x);

int main(void)
{
    // initializing AppEKG library
    EKG_INITIALIZE(3, 1, 103, 53, 13, 1);
    EKG_NAME_HEARTBEAT(1,"BinarySearch");
    EKG_NAME_HEARTBEAT(2,"LinearSearch");
    EKG_NAME_HEARTBEAT(3,"LinearRecSearch");
    // declare array and initialize values
    int n = ARRAYSIZE;
    int arr[n];
    for (int i=0;i<n;i++) {
        arr[i] = i;
    }
    // select random x value for search operation
    srand(time(0));
    int x = rand() % (n*2); // fail about 50% of time
    // perform binary search operation and storing the result
    int resultBinary = binarySearch(arr, 0, n - 1, x);
    // perform linear search operation and storing the result
    int resultLinear = linearSearch(arr, n, x);
    // perform linear search operation recursively and storing the result
    int resultLinearRecursive = linearSearchRecursive(arr, n, x);
    // print the result
    if (resultBinary == -1) 
        printf("Binary Search: Element %d is not present in array\n", x); 
    else 
        printf("Binary Search: Element %d is present at index %d\n", x, resultBinary);
    if (resultLinear == -1) 
        printf("Linear Search: Element %d is not present in array\n", x); 
    else 
        printf("Linear Search: Element %d is present at index %d\n", x, resultLinear);
    if (resultLinearRecursive == -1) 
        printf("Linear Search (Recursive): Element %d is not present in array\n", x); 
    else 
        printf("Linear Search (Recursive): Element %d is present at index %d\n", x,
               resultLinearRecursive);
    // finalize/close AppEKG library
    EKG_FINALIZE();
    return 0;
}

int linearSearch(int arr[], int n, int x)
{
    for (int i=0; i < n; i++) {
        // start HB tracking
        EKG_BEGIN_HEARTBEAT(2, 1);
        if (DELAY > 0) usleep(DELAY);
        if (arr[i]==x) {
            // end HB tracking
            EKG_END_HEARTBEAT(2);
            return i;
        }
        // end HB tracking
        EKG_END_HEARTBEAT(2);
    }
    return -1;
}

// Recursive function to search x in arr[]
int linearSearchRecursive(int arr[], int size, int x) 
{
    // start HB tracking
    EKG_BEGIN_HEARTBEAT(3, 1);
    int recursive;
    size--;
    if (DELAY > 0) usleep(DELAY);
    if (size >= 0) {
        if (arr[size] == x) {
            // end HB tracking
            EKG_END_HEARTBEAT(3);
            return size;
        }
        else {
            // end HB tracking
            EKG_END_HEARTBEAT(3);
            recursive = linearSearchRecursive(arr, size, x);
        }
    }
    else {
        // end HB tracking
        EKG_END_HEARTBEAT(3);
        return -1;
    }
    // end HB tracking
    EKG_END_HEARTBEAT(3);
    return recursive;
}

int binarySearch(int arr[], int l, int r, int x)
{
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
    // end HB tracking
    EKG_END_HEARTBEAT(1);
    // We reach here when element is not present in array
    return -1;
}

