
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <appekg.h>

#define HB1_RATE 1
#define HB2_RATE 1
#define HB3_RATE 1

int numLists;
int listSize;

double** data=0;

int main(int argc, char *argv[])
{
   int numLists = (512*1024);
   int listSize = 256;
   int iterations = 50;
   int randomSeed = 42;
   double sum=0;
   unsigned int i,j,k;
   i = 1; // start at first arg after command name
   while (i < argc-1) // all our options need two args, so don't process the last one
   {
      if (!strcmp(argv[i],"-n")) {
         numLists = strtol(argv[i+1],0,10);
      } else if (!strcmp(argv[i],"-l")) {
         listSize = strtol(argv[i+1],0,10);
      } else if (!strcmp(argv[i],"-i")) {
         iterations = strtol(argv[i+1],0,10);
      } else if (!strcmp(argv[i],"-r")) {
         randomSeed = strtol(argv[i+1],0,10);
      } else {
         fprintf(stderr,"Unknown option (%s)\n", argv[i]);
         i--; // force skip of one arg 
      }
      i += 2; // assume used two args
   }
   fprintf(stdout,"number of lists: %d\n", numLists);
   fprintf(stdout,"size of lists  : %d\n", listSize);
   fprintf(stdout,"num iterations : %d\n", iterations);
   fprintf(stdout,"random seed    : %d\n", randomSeed);
   fprintf(stdout,"openmp threads : %d\n", omp_get_max_threads());
   fprintf(stdout,"initializing...\n");
   EKG_INITIALIZE(3, 1, 1, 1, 1, 0);
   EKG_NAME_HEARTBEAT(1, "Init");
   EKG_NAME_HEARTBEAT(2, "Modify");
   EKG_NAME_HEARTBEAT(3, "Sum");
   srandom(randomSeed);
   data = (double**) malloc(sizeof(double*)*numLists);
   #pragma omp parallel for private(i,j)
   for (i=0; i < numLists; i++) {
      EKG_BEGIN_HEARTBEAT(1,HB1_RATE);
      data[i] = (double*) malloc (sizeof(double)*listSize);
      for (j=0; j < listSize; j++)
         data[i][j] = random();
      EKG_END_HEARTBEAT(1);
   }
   fprintf(stdout,"modifying...\n");
   for (k=0; k < iterations; k++) {
      #pragma omp parallel for private(i,j)
      for (i=0; i < numLists; i++) {
         EKG_BEGIN_HEARTBEAT(2,HB2_RATE);
         for (j=1; j < listSize; j++) {
            if ((i%2)==0)
               data[i][j] += data[i][j-1];
            else
               data[i][j] -= data[i][j-1];
         }
         EKG_END_HEARTBEAT(2);
      }
   }
   fprintf(stdout,"summing...\n");
   sum = 0;
   #pragma omp parallel for private(i,j)
   for (i=0; i < numLists; i++) {
      EKG_BEGIN_HEARTBEAT(3,HB3_RATE);
      for (j=0; j < listSize; j++)
         sum += data[i][j];
      EKG_END_HEARTBEAT(3);
   }
   EKG_FINALIZE();
   fprintf(stdout,"final sum: %g\n", sum);
   return 0;
}

