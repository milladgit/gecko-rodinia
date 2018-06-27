#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "timer.h"

void run(int argc, char** argv);

/* define timer macros */
#define pin_stats_reset()   startCycle()
#define pin_stats_pause(cycles)   stopCycle(cycles)
#define pin_stats_dump(cycles)    printf("timer: %Lu\n", cycles)

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
	if(argc==3){
		cols = atoi(argv[1]);
		rows = atoi(argv[2]);
	}else{
                printf("Usage: pathfiner width num_of_steps\n");
                exit(0);
        }
//	data = new int[rows*cols];
//	result = new int[cols];
#pragma gecko memory allocate(data[0:rows*cols]) type(int) location(exec_loc)
#pragma gecko memory allocate(result[0:cols]) type(int) location(exec_loc)

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
#pragma gecko config env

    init(argc, argv);

    unsigned long long cycles;

    int *src, *dst, *temp;
    int min;

    dst = result;
//    src = new int[cols];
#pragma gecko memory allocate(src[0:cols]) type(int) location(exec_loc)

    pin_stats_reset();
//    #pragma acc data create(src[0:cols]) copy(dst[0:cols], data[0:rows*cols])

    {
    for (int t = 0; t < rows-1; t++) {
        temp = src;
        src = dst;
        dst = temp;

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen) variable_list(src,dst,data)
        #pragma acc parallel loop independent
        for(int n = 0; n < cols; n++){
          min = src[n];
          if (n > 0)
            min = MIN(min, src[n-1]);
          if (n < cols-1)
            min = MIN(min, src[n+1]);
          dst[n] = wall(t+1,n)+min;
        }
#pragma gecko region end

    }
    } /* end pragma acc data */

#pragma gecko region pause at(exec_loc)


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
#pragma gecko memory free(data)
#pragma gecko memory free(dst)
#pragma gecko memory free(src)
}

