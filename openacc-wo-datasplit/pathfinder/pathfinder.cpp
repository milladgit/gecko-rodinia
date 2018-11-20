#include "geckoRuntime.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "timer.h"

void run(int argc, char** argv);

/* define timer macros */
#define pin_stats_reset()   startTime()
#define pin_stats_pause(cycles)   stopTime(cycles)
#define pin_stats_dump(cycles)    printf("timer: %Luus\n", cycles)

//#define BENCH_PRINT

int rows, cols;
int* data;
#define wall(i,j) (data[i*rows+j])
int* result;
#define M_SEED 9


static char *exec_loc = "LocB";
static char *exec_policy_chosen = "static";


void
init(int argc, char** argv)
{
    if(argc==3) {
        cols = atoi(argv[1]);
        rows = atoi(argv[2]);
    } else {
        printf("Usage: pathfiner width num_of_steps\n");
        exit(0);
    }
//	data = new int[rows*cols];
//	result = new int[cols];
//#pragma gecko memory allocate(data[0:rows*cols]) type(int) location(exec_loc)
//#pragma gecko memory allocate(result[0:cols]) type(int) location(exec_loc)
    geckoMemoryDeclare((void**)&data, sizeof(int), rows*cols, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&result, sizeof(int), cols, exec_loc, GECKO_DISTANCE_NOT_SET);

    int seed = M_SEED;
    srand(seed);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            wall(i,j) = rand() % 10;
        }
    }
    for (int j = 0; j < cols; j++)
        result[j] = wall(0,j);
#ifdef BENCH_PRINT
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%d ",wall(i,j)) ;
        }
        printf("\n") ;
    }
#endif
}

void
fatal(char *s)
{
    fprintf(stderr, "error: %s\n", s);

}

#define IN_RANGE(x, min, max)   ((x)>=(min) && (x)<=(max))
#define CLAMP_RANGE(x, min, max) x = (x<(min)) ? min : ((x>(max)) ? max : x )
#define MIN(a, b) ((a)<=(b) ? (a) : (b))

int main(int argc, char** argv)
{
    run(argc,argv);

    return EXIT_SUCCESS;
}

void run(int argc, char** argv)
{
    geckoLoadConfigWithEnv();
    init(argc, argv);

    unsigned long long cycles;

//    int *src, *dst, *temp;
    int *src, *dst, *temp;
    int min;

    pin_stats_reset();

    dst = result;
//    src = new int[cols];
    geckoMemoryDeclare((void**)&src, sizeof(int), cols, exec_loc, GECKO_DISTANCE_NOT_SET);

//    #pragma acc data create(src[0:cols]) copy(dst[0:cols], data[0:rows*cols])

    {
        for (int t = 0; t < rows-1; t++) {
            temp = src;
            src = dst;
            dst = temp;

            {
                int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
                GeckoLocation **dev = NULL;
                int ranges_count = 0;
                float *ranges = NULL;
                int var_count = 3;
                void **var_list = (void **) malloc(sizeof(void*) * var_count);
                for(int __v_id=0; __v_id<var_count; __v_id++) {
                    var_list[__v_id] = src;
                    var_list[__v_id] = dst;
                    var_list[__v_id] = data;
                }
                GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, cols, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
                jobCount = devCount;
                if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                    #pragma omp parallel num_threads(jobCount)
                    {
                        int devIndex = omp_get_thread_num();
                        if(dev[devIndex] != NULL) {
                            int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                            int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop independent deviceptr(src,dst,data) async(asyncID) copyin()
                            for(int n = beginLI; n < endLI; n++) {
                                min = src[n];
                                if (n > 0)
                                    min = MIN(min, src[n-1]);
                                if (n < cols-1)
                                    min = MIN(min, src[n+1]);
                                dst[n] = wall(t+1,n)+min;
                            }
#pragma acc wait(asyncID)
                        } // end of if(dev[devIndex]!=NULL)
                    } // end of OpenMP pragma
                } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
                geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
            }

        }
    } /* end pragma acc data */

    geckoWaitOnLocation(exec_loc);


    pin_stats_pause(cycles);
    pin_stats_dump(cycles);

#ifdef BENCH_PRINT

    for (int i = 0; i < cols; i++)

        printf("%d ",data[i]) ;

    printf("\n") ;

    for (int i = 0; i < cols; i++)

        printf("%d ",dst[i]) ;

    printf("\n") ;

#endif

//    delete [] data;
//    delete [] dst;
//    delete [] src;
    geckoFree(data);
    geckoFree(dst);
    geckoFree(src);
}

