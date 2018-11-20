#include "geckoRuntime.h"
// srad.cpp : Defines the entry point for the console application.
//

//#define OUTPUT


#define OPEN
#define	ITERATION
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>

static char *exec_loc = "LocB";
static char *exec_policy_chosen = "static";

void random_matrix(float *I, int rows, int cols);

void usage(int argc, char **argv)
{
    fprintf(stderr, "Usage: %s <rows> <cols> <y1> <y2> <x1> <x2> <lamda> <no. of iter>\n", argv[0]);
    fprintf(stderr, "\t<rows>   - number of rows\n");
    fprintf(stderr, "\t<cols>    - number of cols\n");
    fprintf(stderr, "\t<y1> 	 - y1 value of the speckle\n");
    fprintf(stderr, "\t<y2>      - y2 value of the speckle\n");
    fprintf(stderr, "\t<x1>       - x1 value of the speckle\n");
    fprintf(stderr, "\t<x2>       - x2 value of the speckle\n");
    fprintf(stderr, "\t<lamda>   - lambda (0,1)\n");
    fprintf(stderr, "\t<no. of iter>   - number of iterations\n");

    exit(1);
}

int main(int argc, char* argv[])
{
    int rows, cols, size_I, size_R, niter = 10, iter, k;
    float *I, *J, q0sqr, sum, sum2, tmp, meanROI,varROI ;
    float Jc, G2, L, num, den, qsqr;
    int *iN,*iS,*jE,*jW;
    float *dN,*dS,*dW,*dE;
    int r1, r2, c1, c2;
    float cN,cS,cW,cE;
    float *c, D;
    float lambda;
    int i, j;

    if (argc == 9)
    {
        rows = atoi(argv[1]); //number of rows in the domain
        cols = atoi(argv[2]); //number of cols in the domain
        if ((rows%16!=0) || (cols%16!=0)) {
            fprintf(stderr, "rows and cols must be multiples of 16\n");
            exit(1);
        }
        r1   = atoi(argv[3]); //y1 position of the speckle
        r2   = atoi(argv[4]); //y2 position of the speckle
        c1   = atoi(argv[5]); //x1 position of the speckle
        c2   = atoi(argv[6]); //x2 position of the speckle
        lambda = atof(argv[7]); //Lambda value
        niter = atoi(argv[8]); //number of iterations
    }
    else {
        usage(argc, argv);
    }

    geckoLoadConfigWithEnv();

    size_I = cols * rows;
    size_R = (r2-r1+1)*(c2-c1+1);

    double total_time;
    total_time = omp_get_wtime();


//	I = (float *)malloc( size_I * sizeof(float) );
//	J = (float *)malloc( size_I * sizeof(float) );
//	c  = (float *)malloc(sizeof(float)* size_I) ;
    geckoMemoryDeclare((void**)&I, sizeof(float), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&J, sizeof(float), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&c, sizeof(float), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);


//	iN = (int *)malloc(sizeof(int) * rows) ;
//	iS = (int *)malloc(sizeof(int) * rows) ;
//	jW = (int *)malloc(sizeof(int) * cols) ;
//	jE = (int *)malloc(sizeof(int) * cols) ;
    geckoMemoryDeclare((void**)&iN, sizeof(int), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&iS, sizeof(int), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&jW, sizeof(int), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&jE, sizeof(int), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);


//	dN = (float *)malloc(sizeof(float)* size_I) ;
//    dS = (float *)malloc(sizeof(float)* size_I) ;
//    dW = (float *)malloc(sizeof(float)* size_I) ;
//    dE = (float *)malloc(sizeof(float)* size_I) ;
    geckoMemoryDeclare((void**)&dN, sizeof(float), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&dS, sizeof(float), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&dW, sizeof(float), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryDeclare((void**)&dE, sizeof(float), size_I, exec_loc, GECKO_DISTANCE_NOT_SET);

//#pragma acc data create(iN[0:rows],iS[0:rows],jW[0:cols],jE[0:cols]) \
//    create(dN[0:size_I],dS[0:size_I],dW[0:size_I],dE[0:size_I],c[0:size_I]) \
//    create(I[0:size_I]) copyout(J[0:size_I])
    {

        {
            int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
            GeckoLocation **dev = NULL;
            int ranges_count = 0;
            float *ranges = NULL;
            int var_count = 2;
            void **var_list = (void **) malloc(sizeof(void*) * var_count);
            for(int __v_id=0; __v_id<var_count; __v_id++) {
                var_list[__v_id] = iN;
                var_list[__v_id] = iS;
            }
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, rows, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr(iN,iS) async(asyncID) copyin()
                        for(int i = beginLI; i < endLI; i++) {
                            iN[i] = i-1;
                            iS[i] = i+1;
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
            int var_count = 2;
            void **var_list = (void **) malloc(sizeof(void*) * var_count);
            for(int __v_id=0; __v_id<var_count; __v_id++) {
                var_list[__v_id] = jW;
                var_list[__v_id] = jE;
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
#pragma acc parallel loop deviceptr(jW,jE) async(asyncID) copyin()
                        for(int j = beginLI; j < endLI; j++) {
                            jW[j] = j-1;
                            jE[j] = j+1;
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
            int var_count = 4;
            void **var_list = (void **) malloc(sizeof(void*) * var_count);
            for(int __v_id=0; __v_id<var_count; __v_id++) {
                var_list[__v_id] = iN;
                var_list[__v_id] = iS;
                var_list[__v_id] = jW;
                var_list[__v_id] = jE;
            }
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, 1, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc serial deviceptr(iN,iS,jW,jE) async(asyncID) copyin()
                        for(int i = beginLI; i < endLI; i++)
                        {
                            iN[0]    = 0;
                            iS[rows-1] = rows-1;
                            jW[0]    = 0;
                            jE[cols-1] = cols-1;
                        }
#pragma acc wait(asyncID)
                    } // end of if(dev[devIndex]!=NULL)
                } // end of OpenMP pragma
            } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
            geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
        }

        geckoWaitOnLocation(exec_loc);

        printf("Randomizing the input matrix\n");

        random_matrix(I, rows, cols);
//    #pragma acc update device(I[0:size_I])

        geckoWaitOnLocation(exec_loc);

        {
            int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
            GeckoLocation **dev = NULL;
            int ranges_count = 0;
            float *ranges = NULL;
            int var_count = 2;
            void **var_list = (void **) malloc(sizeof(void*) * var_count);
            for(int __v_id=0; __v_id<var_count; __v_id++) {
                var_list[__v_id] = I;
                var_list[__v_id] = J;
            }
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, size_I, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr(I,J) async(asyncID) copyin()
                        for( k = beginLI; k < endLI; k++) {
                            J[k] = (float)exp(I[k]) ;
                        }
#pragma acc wait(asyncID)
                    } // end of if(dev[devIndex]!=NULL)
                } // end of OpenMP pragma
            } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
            geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
        }
//#pragma gecko region pause at(exec_loc)

        printf("Start the SRAD main loop\n");

        double main_loop_time;
        main_loop_time = omp_get_wtime();


#ifdef ITERATION
        for (iter=0; iter< niter; iter++) {
#endif
            sum=0;
            sum2=0;
            {
                int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
                GeckoLocation **dev = NULL;
                int ranges_count = 0;
                float *ranges = NULL;
                int var_count = 2;
                void **var_list = (void **) malloc(sizeof(void*) * var_count);
                for(int __v_id=0; __v_id<var_count; __v_id++) {
                    var_list[__v_id] = I;
                    var_list[__v_id] = J;
                }
                GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, r1, r2, 1, 1, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
                jobCount = devCount;
                if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                    #pragma omp parallel num_threads(jobCount)
                    {
                        int devIndex = omp_get_thread_num();
                        if(dev[devIndex] != NULL) {
                            int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                            int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop collapse(2) reduction(+:sum,sum2) deviceptr(I,J) async(asyncID) copyin()
                            for( i = beginLI; i <= endLI; i++) {
                                for (j=c1; j<=c2; j++) {
                                    tmp   = J[i * cols + j];
                                    sum  += tmp ;
                                    sum2 += tmp*tmp;
                                }
                            }
#pragma acc wait(asyncID)
                        } // end of if(dev[devIndex]!=NULL)
                    } // end of OpenMP pragma
                } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
                geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
            }
//#pragma gecko region pause at(exec_loc)

            meanROI = sum / size_R;
            varROI  = (sum2 / size_R) - meanROI*meanROI;
            q0sqr   = varROI / (meanROI*meanROI);


            {
                int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
                GeckoLocation **dev = NULL;
                int ranges_count = 0;
                float *ranges = NULL;
                int var_count = 11;
                void **var_list = (void **) malloc(sizeof(void*) * var_count);
                for(int __v_id=0; __v_id<var_count; __v_id++) {
                    var_list[__v_id] = I;
                    var_list[__v_id] = J;
                    var_list[__v_id] = c;
                    var_list[__v_id] = dN;
                    var_list[__v_id] = dS;
                    var_list[__v_id] = dW;
                    var_list[__v_id] = dE;
                    var_list[__v_id] = iN;
                    var_list[__v_id] = iS;
                    var_list[__v_id] = jW;
                    var_list[__v_id] = jE;
                }
                GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, rows, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
                jobCount = devCount;
                if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                    #pragma omp parallel num_threads(jobCount)
                    {
                        int devIndex = omp_get_thread_num();
                        if(dev[devIndex] != NULL) {
                            int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                            int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop collapse(2) independent private(k) deviceptr(I,J,c,dN,dS,dW,dE,iN,iS,jW,jE) async(asyncID) copyin()
                            for(int i = beginLI; i < endLI; i++) {
                                for (int j = 0; j < cols; j++) {

                                    float dNk, dSk, dWk, dEk, ck;

                                    k = i * cols + j;
                                    Jc = J[k];

                                    // directional derivates
                                    dNk = J[iN[i] * cols + j] - Jc;
                                    dSk = J[iS[i] * cols + j] - Jc;
                                    dWk = J[i * cols + jW[j]] - Jc;
                                    dEk = J[i * cols + jE[j]] - Jc;

                                    G2 = (dNk*dNk + dSk*dSk
                                          + dWk*dWk + dEk*dEk) / (Jc*Jc);

                                    L = (dNk + dSk + dWk + dEk) / Jc;

                                    num  = (0.5*G2) - ((1.0/16.0)*(L*L)) ;
                                    den  = 1 + (.25*L);
                                    qsqr = num/(den*den);

                                    // diffusion coefficent (equ 33)
                                    den = (qsqr-q0sqr) / (q0sqr * (1+q0sqr)) ;
                                    ck = 1.0 / (1.0+den) ;

                                    // saturate diffusion coefficent
                                    if (ck < 0) {
                                        c[k] = 0;
                                    }
                                    else if (ck >= 1) {
                                        c[k] = 1;
                                    }

                                    dN[k] = dNk, dS[k] = dSk, dW[k] = dWk, dE[k] = dEk;

                                }
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
                int var_count = 9;
                void **var_list = (void **) malloc(sizeof(void*) * var_count);
                for(int __v_id=0; __v_id<var_count; __v_id++) {
                    var_list[__v_id] = I;
                    var_list[__v_id] = J;
                    var_list[__v_id] = c;
                    var_list[__v_id] = dN;
                    var_list[__v_id] = dS;
                    var_list[__v_id] = dW;
                    var_list[__v_id] = dE;
                    var_list[__v_id] = iS;
                    var_list[__v_id] = jE;
                }
                GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, rows, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
                jobCount = devCount;
                if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                    #pragma omp parallel num_threads(jobCount)
                    {
                        int devIndex = omp_get_thread_num();
                        if(dev[devIndex] != NULL) {
                            int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                            int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop collapse(2) independent private(k) deviceptr(I,J,c,dN,dS,dW,dE,iS,jE) async(asyncID) copyin()
                            for(int i = beginLI; i < endLI; i++) {
                                for (int j = 0; j < cols; j++) {

                                    // current index
                                    k = i * cols + j;

                                    // diffusion coefficent
                                    cN = c[k];
                                    cS = c[iS[i] * cols + j];
                                    cW = c[k];
                                    cE = c[i * cols + jE[j]];

                                    // divergence (equ 58)
                                    D = cN * dN[k] + cS * dS[k] + cW * dW[k] + cE * dE[k];

                                    // image update (equ 61)
                                    J[k] = J[k] + 0.25*lambda*D;
                                }
                            }
#pragma acc wait(asyncID)
                        } // end of if(dev[devIndex]!=NULL)
                    } // end of OpenMP pragma
                } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
                geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
            }
//#pragma gecko region pause at(exec_loc)

#ifdef ITERATION
        }
#endif

        main_loop_time = omp_get_wtime();
        printf("Main Loop Time: %.2fus\n", main_loop_time*1E6);

    } /* end acc data */

    total_time = omp_get_wtime();
    printf("Total Time: %.2fus\n", total_time*1E6);

#ifdef OUTPUT
    for( int i = 0 ; i < rows ; i++) {
        for ( int j = 0 ; j < cols ; j++) {

            printf("%.5f ", J[i * cols + j]);

        }
        printf("\n");
    }
#endif

    printf("Computation Done\n");

//	free(I);
//	free(J);
//	free(iN); free(iS); free(jW); free(jE);
//    free(dN); free(dS); free(dW); free(dE);
    geckoFree(I);
    geckoFree(J);
    geckoFree(iN);
    geckoFree(iS);
    geckoFree(jW);
    geckoFree(jE);
    geckoFree(dN);
    geckoFree(dS);
    geckoFree(dW);
    geckoFree(dE);


//	free(c);
    geckoFree(c);
    return 0;
}




void random_matrix(float *I, int rows, int cols) {

    srand(7);

    for( int i = 0 ; i < rows ; i++) {
        for ( int j = 0 ; j < cols ; j++) {
            I[i * cols + j] = rand()/(float)RAND_MAX ;
#ifdef OUTPUT
            //printf("%g ", I[i * cols + j]);
#endif
        }
#ifdef OUTPUT
        //printf("\n");
#endif
    }

}

