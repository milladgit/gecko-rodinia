#include "geckoRuntime.h"
// #ifdef __cplusplus
// extern "C" {
// #endif

//========================================================================================================================================================================================================200
//	DEFINE/INCLUDE
//========================================================================================================================================================================================================200

//======================================================================================================================================================150
//	LIBRARIES
//======================================================================================================================================================150

#include <stdlib.h>									// (in directory known to compiler)

//======================================================================================================================================================150
//	COMMON
//======================================================================================================================================================150

#include "../common.h"								// (in directory provided here)

//======================================================================================================================================================150
//	UTILITIES
//======================================================================================================================================================150

#include "../util/timer/timer.h"					// (in directory provided here)	needed by timer

//======================================================================================================================================================150
//	HEADER
//======================================================================================================================================================150

#include "./kernel_cpu_2.h"							// (in directory provided here)

//========================================================================================================================================================================================================200
//	PLASMAKERNEL_GPU
//========================================================================================================================================================================================================200

void
kernel_cpu_2(	int cores_arg,

                knode *knodes,
                long knodes_elem,

                int order,
                long maxheight,
                int count,

                long *currKnode,
                long *offset,
                long *lastKnode,
                long *offset_2,
                int *start,
                int *end,
                int *recstart,
                int *reclength)
{

    //======================================================================================================================================================150
    //	Variables
    //======================================================================================================================================================150

    // timer
    long long time0;
    long long time1;
    long long time2;

    // common variables
    int i;

    time0 = get_time();

    //======================================================================================================================================================150
    //	MCPU SETUP
    //======================================================================================================================================================150

    int threadsPerBlock;
    threadsPerBlock = order < 1024 ? order : 1024;

    time1 = get_time();

    //======================================================================================================================================================150
    //	PROCESS INTERACTIONS
    //======================================================================================================================================================150

    // private thread IDs
    int thid;
    int bid;

    // process number of querries
    {
        int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
        GeckoLocation **dev = NULL;
        int ranges_count = 0;
        float *ranges = NULL;
        int var_count = 9;
        void **var_list = (void **) malloc(sizeof(void*) * var_count);
        for(int __v_id=0; __v_id<var_count; __v_id++) {
            var_list[__v_id] = knodes;
            var_list[__v_id] = currKnode;
            var_list[__v_id] = lastKnode;
            var_list[__v_id] = offset;
            var_list[__v_id] = offset_2;
            var_list[__v_id] = start;
            var_list[__v_id] = end;
            var_list[__v_id] = recstart;
            var_list[__v_id] = reclength;
        }
        GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, count, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
        jobCount = devCount;
        if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
            #pragma omp parallel num_threads(jobCount)
            {
                int devIndex = omp_get_thread_num();
                if(dev[devIndex] != NULL) {
                    int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                    int asyncID = dev[devIndex]->getAsyncID();
#pragma acc kernels present(knodes,currKnode,lastKnode,offset,offset_2,start,end) private(thid,bid,i) deviceptr(knodes,currKnode,lastKnode,offset,offset_2,start,end,recstart,reclength) async(asyncID) copyin()
                    for( bid = beginLI; bid < endLI; bid++) {

                        // process levels of the tree
                        for(i = 0; i < maxheight; i++) {

                            // process all leaves at each level
                            for(thid = 0; thid < threadsPerBlock; thid++) {

                                if((knodes[currKnode[bid]].keys[thid] <= start[bid]) && (knodes[currKnode[bid]].keys[thid+1] > start[bid])) {
                                    // this conditional statement is inserted to avoid crush due to but in original code
                                    // "offset[bid]" calculated below that later addresses part of knodes goes outside of its bounds cause segmentation fault
                                    // more specifically, values saved into knodes->indices in the main function are out of bounds of knodes that they address
                                    if(knodes[currKnode[bid]].indices[thid] < knodes_elem) {
                                        offset[bid] = knodes[currKnode[bid]].indices[thid];
                                    }
                                }
                                if((knodes[lastKnode[bid]].keys[thid] <= end[bid]) && (knodes[lastKnode[bid]].keys[thid+1] > end[bid])) {
                                    // this conditional statement is inserted to avoid crush due to but in original code
                                    // "offset_2[bid]" calculated below that later addresses part of knodes goes outside of its bounds cause segmentation fault
                                    // more specifically, values saved into knodes->indices in the main function are out of bounds of knodes that they address
                                    if(knodes[lastKnode[bid]].indices[thid] < knodes_elem) {
                                        offset_2[bid] = knodes[lastKnode[bid]].indices[thid];
                                    }
                                }

                            }

                            // set for next tree level
                            currKnode[bid] = offset[bid];
                            lastKnode[bid] = offset_2[bid];

                        }

                    }
#pragma acc wait(asyncID)
                } // end of if(dev[devIndex]!=NULL)
            } // end of OpenMP pragma
        } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
        geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
    }

    {
        int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
        GeckoLocation **dev = NULL;
        int ranges_count = 0;
        float *ranges = NULL;
        int var_count = 9;
        void **var_list = (void **) malloc(sizeof(void*) * var_count);
        for(int __v_id=0; __v_id<var_count; __v_id++) {
            var_list[__v_id] = knodes;
            var_list[__v_id] = currKnode;
            var_list[__v_id] = lastKnode;
            var_list[__v_id] = offset;
            var_list[__v_id] = offset_2;
            var_list[__v_id] = start;
            var_list[__v_id] = end;
            var_list[__v_id] = recstart;
            var_list[__v_id] = reclength;
        }
        GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, count, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
        jobCount = devCount;
        if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
            #pragma omp parallel num_threads(jobCount)
            {
                int devIndex = omp_get_thread_num();
                if(dev[devIndex] != NULL) {
                    int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                    int asyncID = dev[devIndex]->getAsyncID();
#pragma acc kernels present(recstart,knodes,currKnode,start,end) deviceptr(knodes,currKnode,lastKnode,offset,offset_2,start,end,recstart,reclength) async(asyncID) copyin()
                    for( bid = beginLI; bid < endLI; bid++) {
                        // process leaves
                        for(thid = 0; thid < threadsPerBlock; thid++) {

                            // Find the index of the starting record
                            if(knodes[currKnode[bid]].keys[thid] == start[bid]) {
                                recstart[bid] = knodes[currKnode[bid]].indices[thid];
                            }

                        }

                    }
#pragma acc wait(asyncID)
                } // end of if(dev[devIndex]!=NULL)
            } // end of OpenMP pragma
        } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
        geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
    }

    {
        int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
        GeckoLocation **dev = NULL;
        int ranges_count = 0;
        float *ranges = NULL;
        int var_count = 9;
        void **var_list = (void **) malloc(sizeof(void*) * var_count);
        for(int __v_id=0; __v_id<var_count; __v_id++) {
            var_list[__v_id] = knodes;
            var_list[__v_id] = currKnode;
            var_list[__v_id] = lastKnode;
            var_list[__v_id] = offset;
            var_list[__v_id] = offset_2;
            var_list[__v_id] = start;
            var_list[__v_id] = end;
            var_list[__v_id] = recstart;
            var_list[__v_id] = reclength;
        }
        GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, count, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
        jobCount = devCount;
        if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
            #pragma omp parallel num_threads(jobCount)
            {
                int devIndex = omp_get_thread_num();
                if(dev[devIndex] != NULL) {
                    int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                    int asyncID = dev[devIndex]->getAsyncID();
#pragma acc kernels present(recstart,knodes,currKnode,start,end,reclength) private(thid) deviceptr(knodes,currKnode,lastKnode,offset,offset_2,start,end,recstart,reclength) async(asyncID) copyin()
                    for( bid = beginLI; bid < endLI; bid++) {
                        // process leaves
                        for(thid = 0; thid < threadsPerBlock; thid++) {

                            // Find the index of the ending record
                            if(knodes[lastKnode[bid]].keys[thid] == end[bid]) {
                                reclength[bid] = knodes[lastKnode[bid]].indices[thid] - recstart[bid]+1;
                            }

                        }

                    }
#pragma acc wait(asyncID)
                } // end of if(dev[devIndex]!=NULL)
            } // end of OpenMP pragma
        } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
        geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
    }

    time2 = get_time();

    //======================================================================================================================================================150
    //	DISPLAY TIMING
    //======================================================================================================================================================150

    printf("Time spent in different stages of CPU/MCPU KERNEL:\n");

    printf("%15.12f s, %15.12f % : MCPU: SET DEVICE\n",					(float) (time1-time0) / 1000000, (float) (time1-time0) / (float) (time2-time0) * 100);
    printf("%15.12f s, %15.12f % : CPU/MCPU: KERNEL\n",					(float) (time2-time1) / 1000000, (float) (time2-time1) / (float) (time2-time0) * 100);

    printf("Total time:\n");
    printf("%.12f s\n", 												(float) (time2-time0) / 1000000);

} // main

//========================================================================================================================================================================================================200
//	END
//========================================================================================================================================================================================================200

// #ifdef __cplusplus
// }
// #endif
