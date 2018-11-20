#include "geckoRuntime.h"
#include <stdio.h>

extern char *exec_loc;
extern char *exec_policy_chosen;

void lud_openacc(float *a, int size)
{
    int i,j,k;
    float sum;
//     #pragma acc data copy(a[0:size*size])
    for (i=0; i <size; i++) {

        {
            int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
            GeckoLocation **dev = NULL;
            int ranges_count = 0;
            float *ranges = NULL;
            int var_count = 1;
            void **var_list = (void **) malloc(sizeof(void*) * var_count);
            for(int __v_id=0; __v_id<var_count; __v_id++) {
                var_list[__v_id] = a;
            }
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, i, size, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr(a) async(asyncID) copyin()
                        for( j = beginLI; j < endLI; j++) {
                            sum=a[i*size+j];
#pragma acc loop seq
                            for (k=0; k<i; k++) sum -= a[i*size+k]*a[k*size+j];
                            a[i*size+j]=sum;
                        }
#pragma acc wait(asyncID)
                    } // end of if(dev[devIndex]!=NULL)
                } // end of OpenMP pragma
            } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
            geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
        }

//#pragma gecko region pause at(exec_loc)



        {
            int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
            GeckoLocation **dev = NULL;
            int ranges_count = 0;
            float *ranges = NULL;
            int var_count = 1;
            void **var_list = (void **) malloc(sizeof(void*) * var_count);
            for(int __v_id=0; __v_id<var_count; __v_id++) {
                var_list[__v_id] = a;
            }
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, i+1, size, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr(a) async(asyncID) copyin()
                        for( j = beginLI; j < endLI; j++) {
                            sum=a[j*size+i];
#pragma acc loop seq
                            for (k=0; k<i; k++) sum -=a[j*size+k]*a[k*size+i];
                            a[j*size+i]=sum/a[i*size+i];
                        }
#pragma acc wait(asyncID)
                    } // end of if(dev[devIndex]!=NULL)
                } // end of OpenMP pragma
            } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
            geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
        }

//#pragma gecko region pause at(exec_loc)
    }
    geckoWaitOnLocation(exec_loc);

}
