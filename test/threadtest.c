/**
* Test program to see how OpenMP and PThreads behave
* in regards to thread IDs. On Ubuntu Linux, it appears
* that PThread IDs are unique for OpenMP threads, but
* OpenMP is unaware if raw PThreads are used. However,
* there may be platforms where OpenMP does not use 
* PThreads underneath, and so we may not be able to
* rely on PThread IDs always.
**/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <omp.h>

// PThread thread "main" function
// - always takes just one void* argument; if you need
//   multiple data values, make a struct
void* threadFunction(void *data)
{
   int val = *((int*)data);
   printf("T%d: Pthread self   ID: %lu\n", val, pthread_self()%47);
   printf("T%d: OpenMP thread num: %d\n", val, omp_get_thread_num());
   return 0;
}

int main(int argc, char **argv)
{
   int i;
   pthread_t tid;
   // test Pthreads
   printf("Testing Pthreads...\n");
   for (i=0; i < 4; i++) {
      // must create new place to store current value of i,
      // since i will change while the thread is running, so
      // cannot just pass a pointer to i itself
      int *vp = (int*) malloc(sizeof(int));
      *vp = i;
      pthread_create(&tid,0,threadFunction,vp);
   }
   sleep(1); // leave time for threads to end
   // test OpenMP, which is an alternative to PThreads, and easier
   // for easy cases of symmetric threads
   printf("Testing OpenMP...\n");
   omp_set_num_threads(4);
   #pragma omp parallel for
   for (i=0; i < 4; i++) {
      printf("L%d: Pthread self   ID: %lu\n", i, pthread_self()%47);
      printf("L%d: OpenMP thread num: %d\n", i, omp_get_thread_num());
   }
}

