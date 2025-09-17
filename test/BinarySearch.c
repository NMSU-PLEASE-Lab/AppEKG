//
// AppEKG example program that does binary search over an array of values.
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

#define ARRAYSIZE 100000
#define DELAY 5000

int binarySearch(int arr[], int l, int r, int x);

int main(void)
{
   // initializing AppEKG library
   EKG_INITIALIZE(1, 1, 102, 52, 12, 1);
   EKG_NAME_HEARTBEAT(1,"BinarySearch");
   // declare array and initialize values
   int n = ARRAYSIZE;
   int arr[n];
   for (int i=0; i < n; i++) {
       arr[i] = i;
   }
   // select random x value for search operation
   srand(time(0));
   int x = rand() % (n*2); // fail about 50% of time
   // perform binary search operation and storing the result
   int result = binarySearch(arr, 0, n - 1, x);
   if(result == -1)
       printf("Element %d is not present in array\n", x);
   else
       printf("Element %d is present at index %d\n", x, result);
   // finalizing/closing AppEKG library
   EKG_FINALIZE();
   return 0;
}

int binarySearch(int arr[], int l, int r, int x) 
{
   // start HB tracking
   EKG_BEGIN_HEARTBEAT(1, 1);
   if (DELAY > 0) usleep(DELAY);
   if (r >= l) {
       int mid = l + (r - l) / 2;
       // if the element is present at the middle itself
       if (arr[mid] == x){
       // end HB tracking
       EKG_END_HEARTBEAT(1);
       return mid;
   }
   // if element is smaller than mid, then it can only be present in left subarray
   if (arr[mid] > x)
   {
       // end HB tracking
       EKG_END_HEARTBEAT(1);
       return binarySearch(arr, l, mid - 1, x);
   } else
      EKG_END_HEARTBEAT(1);
      // else the element can only be present in right subarray
      return binarySearch(arr, mid + 1, r, x);
   }
    // End HB tracking
    EKG_END_HEARTBEAT(1);
    // We reach here when element is not present in array
    return -1;
}

