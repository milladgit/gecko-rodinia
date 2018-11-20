#include "geckoRuntime.h"
#define LIMIT -999
//#define TRACE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

////////////////////////////////////////////////////////////////////////////////
// declaration, forward
void runTest( int argc, char** argv);
#define MAXOF2(a,b) (((a)>(b))?(a):(b))
#define MAXIMUM(a,b,c) (MAXOF2(MAXOF2(a,b),c))


int blosum62[24][24] = {
    { 4, -1, -2, -2,  0, -1, -1,  0, -2, -1, -1, -1, -1, -2, -1,  1,  0, -3, -2,  0, -2, -1,  0, -4},
    {-1,  5,  0, -2, -3,  1,  0, -2,  0, -3, -2,  2, -1, -3, -2, -1, -1, -3, -2, -3, -1,  0, -1, -4},
    {-2,  0,  6,  1, -3,  0,  0,  0,  1, -3, -3,  0, -2, -3, -2,  1,  0, -4, -2, -3,  3,  0, -1, -4},
    {-2, -2,  1,  6, -3,  0,  2, -1, -1, -3, -4, -1, -3, -3, -1,  0, -1, -4, -3, -3,  4,  1, -1, -4},
    { 0, -3, -3, -3,  9, -3, -4, -3, -3, -1, -1, -3, -1, -2, -3, -1, -1, -2, -2, -1, -3, -3, -2, -4},
    {-1,  1,  0,  0, -3,  5,  2, -2,  0, -3, -2,  1,  0, -3, -1,  0, -1, -2, -1, -2,  0,  3, -1, -4},
    {-1,  0,  0,  2, -4,  2,  5, -2,  0, -3, -3,  1, -2, -3, -1,  0, -1, -3, -2, -2,  1,  4, -1, -4},
    { 0, -2,  0, -1, -3, -2, -2,  6, -2, -4, -4, -2, -3, -3, -2,  0, -2, -2, -3, -3, -1, -2, -1, -4},
    {-2,  0,  1, -1, -3,  0,  0, -2,  8, -3, -3, -1, -2, -1, -2, -1, -2, -2,  2, -3,  0,  0, -1, -4},
    {-1, -3, -3, -3, -1, -3, -3, -4, -3,  4,  2, -3,  1,  0, -3, -2, -1, -3, -1,  3, -3, -3, -1, -4},
    {-1, -2, -3, -4, -1, -2, -3, -4, -3,  2,  4, -2,  2,  0, -3, -2, -1, -2, -1,  1, -4, -3, -1, -4},
    {-1,  2,  0, -1, -3,  1,  1, -2, -1, -3, -2,  5, -1, -3, -1,  0, -1, -3, -2, -2,  0,  1, -1, -4},
    {-1, -1, -2, -3, -1,  0, -2, -3, -2,  1,  2, -1,  5,  0, -2, -1, -1, -1, -1,  1, -3, -1, -1, -4},
    {-2, -3, -3, -3, -2, -3, -3, -3, -1,  0,  0, -3,  0,  6, -4, -2, -2,  1,  3, -1, -3, -3, -1, -4},
    {-1, -2, -2, -1, -3, -1, -1, -2, -2, -3, -3, -1, -2, -4,  7, -1, -1, -4, -3, -2, -2, -1, -2, -4},
    { 1, -1,  1,  0, -1,  0,  0,  0, -1, -2, -2,  0, -1, -2, -1,  4,  1, -3, -2, -2,  0,  0,  0, -4},
    { 0, -1,  0, -1, -1, -1, -1, -2, -2, -1, -1, -1, -1, -2, -1,  1,  5, -2, -2,  0, -1, -1,  0, -4},
    {-3, -3, -4, -4, -2, -2, -3, -2, -2, -3, -2, -3, -1,  1, -4, -3, -2, 11,  2, -3, -4, -3, -2, -4},
    {-2, -2, -2, -3, -2, -1, -2, -3,  2, -1, -1, -2, -1,  3, -3, -2, -2,  2,  7, -1, -3, -2, -1, -4},
    { 0, -3, -3, -3, -1, -2, -2, -3, -3,  3,  1, -2,  1, -1, -2, -2,  0, -3, -1,  4, -3, -2, -1, -4},
    {-2, -1,  3,  4, -3,  0,  1, -1,  0, -3, -4,  0, -3, -3, -2,  0, -1, -4, -3, -3,  4,  1, -1, -4},
    {-1,  0,  0,  1, -3,  3,  4, -2,  0, -3, -3,  1, -1, -3, -1,  0, -1, -3, -2, -2,  1,  4, -1, -4},
    { 0, -1, -1, -1, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2,  0,  0, -2, -1, -1, -1, -1, -1, -4},
    {-4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4,  1}
};

double gettime() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec+t.tv_usec*1e-6;
}

static char *exec_loc = "LocB";
static char *exec_policy_chosen = "static";

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int
main( int argc, char** argv)
{
    runTest( argc, argv);

    return EXIT_SUCCESS;
}

void usage(int argc, char **argv)
{
    fprintf(stderr, "Usage: %s <max_rows/max_cols> <penalty> <num_threads>\n", argv[0]);
    fprintf(stderr, "\t<dimension>      - x and y dimensions\n");
    fprintf(stderr, "\t<penalty>        - penalty(positive integer)\n");
    fprintf(stderr, "\t<num_threads>    - no. of threads\n");
    exit(1);
}

////////////////////////////////////////////////////////////////////////////////
//! Run a simple test for CUDA
////////////////////////////////////////////////////////////////////////////////
void
runTest( int argc, char** argv)
{
    geckoLoadConfigWithEnv();
    int max_rows, max_cols, penalty,idx, index;
    int *input_itemsets, *output_itemsets, *referrence;
    int *matrix_cuda, *matrix_cuda_out, *referrence_cuda;
    int size;


    // the lengths of the two sequences should be able to divided by 16.
    // And at current stage  max_rows needs to equal max_cols
    if (argc == 3)
    {
        max_rows = atoi(argv[1]);
        max_cols = atoi(argv[1]);
        penalty = atoi(argv[2]);
    }
    else {
        usage(argc, argv);
    }

    max_rows = max_rows + 1;
    max_cols = max_cols + 1;
//	referrence = (int *)malloc( max_rows * max_cols * sizeof(int) );
//    input_itemsets = (int *)malloc( max_rows * max_cols * sizeof(int) );
//	output_itemsets = (int *)malloc( max_rows * max_cols * sizeof(int) );

    int total_elements = max_rows*max_cols;
    geckoMemoryDeclare((void**)&referrence, sizeof(int), total_elements, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&input_itemsets, sizeof(int), total_elements, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&output_itemsets, sizeof(int), total_elements, exec_loc, GECKO_DISTANCE_NOT_SET);


    if (!input_itemsets)
        fprintf(stderr, "error: can not allocate memory");

    srand ( 7 );

    double time;
    time = omp_get_wtime();

    for (int i = 0 ; i < max_cols; i++) {
        for (int j = 0 ; j < max_rows; j++) {
            input_itemsets[i*max_cols+j] = 0;
        }
    }

    printf("Start Needleman-Wunsch\n");

    for( int i=1; i< max_rows ; i++) {   //please define your own sequence.
        input_itemsets[i*max_cols] = rand() % 10 + 1;
    }
    for( int j=1; j< max_cols ; j++) {   //please define your own sequence.
        input_itemsets[j] = rand() % 10 + 1;
    }


    for (int i = 1 ; i < max_cols; i++) {
        for (int j = 1 ; j < max_rows; j++) {
            referrence[i*max_cols+j] = blosum62[input_itemsets[i*max_cols]][input_itemsets[j]];
        }
    }

//	#pragma acc data copy(input_itemsets[0:max_rows*max_cols]) \
//	    copyin(referrence[0:max_rows*max_cols])
    {

        {
            int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
            GeckoLocation **dev = NULL;
            int ranges_count = 0;
            float *ranges = NULL;
            int var_count = 1;
            void **var_list = (void **) malloc(sizeof(void*) * var_count);
            for(int __v_id=0; __v_id<var_count; __v_id++) {
                var_list[__v_id] = input_itemsets;
            }
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 1, max_rows, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr(input_itemsets) async(asyncID) copyin()
                        for(int i = beginLI; i < endLI; i++)
                            input_itemsets[i*max_cols] = -i * penalty;
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
            int var_count = 1;
            void **var_list = (void **) malloc(sizeof(void*) * var_count);
            for(int __v_id=0; __v_id<var_count; __v_id++) {
                var_list[__v_id] = input_itemsets;
            }
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 1, max_cols, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr(input_itemsets) async(asyncID) copyin()
                        for(int j = beginLI; j < endLI; j++)
                            input_itemsets[j] = -j * penalty;
#pragma acc wait(asyncID)
                    } // end of if(dev[devIndex]!=NULL)
                } // end of OpenMP pragma
            } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
            geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
        }


        //Compute top-left matrix
        printf("Processing top-left matrix\n");

        for( int i = 0 ; i < max_cols-2 ; i++) {
            {
                int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
                GeckoLocation **dev = NULL;
                int ranges_count = 0;
                float *ranges = NULL;
                int var_count = 2;
                void **var_list = (void **) malloc(sizeof(void*) * var_count);
                for(int __v_id=0; __v_id<var_count; __v_id++) {
                    var_list[__v_id] = input_itemsets;
                    var_list[__v_id] = referrence;
                }
                GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, i, 1, 1, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
                jobCount = devCount;
                if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                    #pragma omp parallel num_threads(jobCount)
                    {
                        int devIndex = omp_get_thread_num();
                        if(dev[devIndex] != NULL) {
                            int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                            int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr(input_itemsets,referrence) async(asyncID) copyin()
                            for( idx = beginLI; idx <= endLI; idx++) {
                                index = (idx + 1) * max_cols + (i + 1 - idx);
                                input_itemsets[index]= MAXIMUM( input_itemsets[index-1-max_cols]+ referrence[index],
                                                                input_itemsets[index-1]         - penalty,
                                                                input_itemsets[index-max_cols]  - penalty);

                            }
#pragma acc wait(asyncID)
                        } // end of if(dev[devIndex]!=NULL)
                    } // end of OpenMP pragma
                } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
                geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
            }
        }

        //Compute bottom-right matrix
        printf("Processing bottom-right matrix\n");

        for( int i = max_cols - 4 ; i >= 0 ; i--) {
            {
                int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
                GeckoLocation **dev = NULL;
                int ranges_count = 0;
                float *ranges = NULL;
                int var_count = 2;
                void **var_list = (void **) malloc(sizeof(void*) * var_count);
                for(int __v_id=0; __v_id<var_count; __v_id++) {
                    var_list[__v_id] = input_itemsets;
                    var_list[__v_id] = referrence;
                }
                GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, i, 1, 1, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
                jobCount = devCount;
                if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                    #pragma omp parallel num_threads(jobCount)
                    {
                        int devIndex = omp_get_thread_num();
                        if(dev[devIndex] != NULL) {
                            int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                            int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr(input_itemsets,referrence) async(asyncID) copyin()
                            for( idx = beginLI; idx <= endLI; idx++) {
                                index =  ( max_cols - idx - 2 ) * max_cols + idx + max_cols - i - 2 ;
                                input_itemsets[index]= MAXIMUM( input_itemsets[index-1-max_cols]+ referrence[index],
                                                                input_itemsets[index-1]         - penalty,
                                                                input_itemsets[index-max_cols]  - penalty);
                            }
#pragma acc wait(asyncID)
                        } // end of if(dev[devIndex]!=NULL)
                    } // end of OpenMP pragma
                } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
                geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
            }
        }


    } /* end pragma acc data */

    time = omp_get_wtime() - time;
    printf("Total time: %.2fus\n", time*1E6);

    geckoWaitOnLocation(exec_loc);

//#define TRACEBACK
#ifdef TRACEBACK

    FILE *fpo = fopen("result.txt","w");
    fprintf(fpo, "print traceback value GPU:\n");

    for (int i = max_rows - 2,  j = max_rows - 2; i>=0, j>=0;) {
        int nw, n, w, traceback;
        if ( i == max_rows - 2 && j == max_rows - 2 )
            fprintf(fpo, "%d ", input_itemsets[ i * max_cols + j]); //print the first element
        if ( i == 0 && j == 0 )
            break;
        if ( i > 0 && j > 0 ) {
            nw = input_itemsets[(i - 1) * max_cols + j - 1];
            w  = input_itemsets[ i * max_cols + j - 1 ];
            n  = input_itemsets[(i - 1) * max_cols + j];
        }
        else if ( i == 0 ) {
            nw = n = LIMIT;
            w  = input_itemsets[ i * max_cols + j - 1 ];
        }
        else if ( j == 0 ) {
            nw = w = LIMIT;
            n  = input_itemsets[(i - 1) * max_cols + j];
        }
        else {
        }

        //traceback = maximum(nw, w, n);
        int new_nw, new_w, new_n;
        new_nw = nw + referrence[i * max_cols + j];
        new_w = w - penalty;
        new_n = n - penalty;

        traceback = MAXIMUM(new_nw, new_w, new_n);
        if(traceback == new_nw)
            traceback = nw;
        if(traceback == new_w)
            traceback = w;
        if(traceback == new_n)
            traceback = n;

        fprintf(fpo, "%d ", traceback);

        if(traceback == nw )
        {
            i--;
            j--;
            continue;
        }

        else if(traceback == w )
        {
            j--;
            continue;
        }

        else if(traceback == n )
        {
            i--;
            continue;
        }

        else
            ;
    }

    fclose(fpo);

#endif

//	free(referrence);
//	free(input_itemsets);
//	free(output_itemsets);

    geckoFree(referrence);
    geckoFree(input_itemsets);
    geckoFree(output_itemsets);

}



