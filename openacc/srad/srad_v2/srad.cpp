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

void random_matrix(gecko_float I, int rows, int cols);

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
    float q0sqr, sum, sum2, tmp, meanROI,varROI ;
	float Jc, G2, L, num, den, qsqr;
//	int *iN,*iS,*jE,*jW;
	gecko_int iN,iS,jE,jW;
//	float *dN,*dS,*dW,*dE;
	gecko_float dN,dS,dW,dE;
	int r1, r2, c1, c2;
	float cN,cS,cW,cE;
	float D;
	float lambda;
	int i, j;
	gecko_float I, J, c;

	if (argc == 9)
	{
		rows = atoi(argv[1]); //number of rows in the domain
		cols = atoi(argv[2]); //number of cols in the domain
		if ((rows%16!=0) || (cols%16!=0)){
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
    else{
		usage(argc, argv);
    }

#pragma gecko config env


	size_I = cols * rows;
    size_R = (r2-r1+1)*(c2-c1+1);

    double total_time;
    total_time = omp_get_wtime();


//	I = (float *)malloc( size_I * sizeof(float) );
//	J = (float *)malloc( size_I * sizeof(float) );
//	c  = (float *)malloc(sizeof(float)* size_I) ;
#pragma gecko memory allocate(I[0:size_I]) type(gecko_float) location(exec_loc)
#pragma gecko memory allocate(J[0:size_I]) type(gecko_float) location(exec_loc)
#pragma gecko memory allocate(c[0:size_I]) type(gecko_float) location(exec_loc)


//	iN = (int *)malloc(sizeof(int) * rows) ;
//	iS = (int *)malloc(sizeof(int) * rows) ;
//	jW = (int *)malloc(sizeof(int) * cols) ;
//	jE = (int *)malloc(sizeof(int) * cols) ;
#pragma gecko memory allocate(iN[0:size_I]) type(gecko_int) location(exec_loc)
#pragma gecko memory allocate(iS[0:size_I]) type(gecko_int) location(exec_loc)
#pragma gecko memory allocate(jW[0:size_I]) type(gecko_int) location(exec_loc)
#pragma gecko memory allocate(jE[0:size_I]) type(gecko_int) location(exec_loc)


//	dN = (float *)malloc(sizeof(float)* size_I) ;
//    dS = (float *)malloc(sizeof(float)* size_I) ;
//    dW = (float *)malloc(sizeof(float)* size_I) ;
//    dE = (float *)malloc(sizeof(float)* size_I) ;
#pragma gecko memory allocate(dN[0:size_I]) type(gecko_float) location(exec_loc)
#pragma gecko memory allocate(dS[0:size_I]) type(gecko_float) location(exec_loc)
#pragma gecko memory allocate(dW[0:size_I]) type(gecko_float) location(exec_loc)
#pragma gecko memory allocate(dE[0:size_I]) type(gecko_float) location(exec_loc)

//#pragma acc data create(iN[0:rows],iS[0:rows],jW[0:cols],jE[0:cols]) \
//    create(dN[0:size_I],dS[0:size_I],dW[0:size_I],dE[0:size_I],c[0:size_I]) \
//    create(I[0:size_I]) copyout(J[0:size_I])
{

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen)
    #pragma acc parallel loop
    for (int i=0; i< rows; i++) {
        iN[i] = i-1;
        iS[i] = i+1;
    }
#pragma gecko region end
//#pragma gecko region pause at(exec_loc)

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen)
    #pragma acc parallel loop
    for (int j=0; j< cols; j++) {
        jW[j] = j-1;
        jE[j] = j+1;
    }
#pragma gecko region end
//#pragma gecko region pause at(exec_loc)

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen)
    #pragma acc serial
	for(int i=0;i<1;i++)
    {
	    iN[0]    = 0;
	    iS[rows-1] = rows-1;
	    jW[0]    = 0;
	    jE[cols-1] = cols-1;
    }
#pragma gecko region end

#pragma gecko region pause at(exec_loc)

	printf("Randomizing the input matrix\n");

    random_matrix(I, rows, cols);
//    #pragma acc update device(I[0:size_I])

#pragma gecko region pause at(exec_loc)

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen)
    #pragma acc parallel loop
    for (k = 0;  k < size_I; k++ ) {
     	J[k] = (float)exp(I[k]) ;
    }
#pragma gecko region end
//#pragma gecko region pause at(exec_loc)

	printf("Start the SRAD main loop\n");

        double main_loop_time;
        main_loop_time = omp_get_wtime();


#ifdef ITERATION
	for (iter=0; iter< niter; iter++){
#endif
		sum=0; sum2=0;
#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen)
		#pragma acc parallel loop collapse(2) reduction(+:sum,sum2)
		for (i=r1; i<=r2; i++) {
            for (j=c1; j<=c2; j++) {
                tmp   = J[i * cols + j];
                sum  += tmp ;
                sum2 += tmp*tmp;
            }
        }
#pragma gecko region end
//#pragma gecko region pause at(exec_loc)

        meanROI = sum / size_R;
        varROI  = (sum2 / size_R) - meanROI*meanROI;
        q0sqr   = varROI / (meanROI*meanROI);


#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen)
        #pragma acc parallel loop collapse(2) independent private(k)
		for (int i = 0 ; i < rows ; i++) {
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
                if (ck < 0) {c[k] = 0;}
                else if (ck >= 1) {c[k] = 1;}

                dN[k] = dNk, dS[k] = dSk, dW[k] = dWk, dE[k] = dEk;

			}
    	}
#pragma gecko region end
//#pragma gecko region pause at(exec_loc)

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen)
    	#pragma acc parallel loop collapse(2) independent private(k)
		for (int i = 0; i < rows; i++) {
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
#pragma gecko region end
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
	  for( int i = 0 ; i < rows ; i++){
		for ( int j = 0 ; j < cols ; j++){

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
//#pragma gecko memory free(I)
//#pragma gecko memory free(J)
//#pragma gecko memory free(iN)
//#pragma gecko memory free(iS)
//#pragma gecko memory free(jW)
//#pragma gecko memory free(jE)
//#pragma gecko memory free(dN)
//#pragma gecko memory free(dS)
//#pragma gecko memory free(dW)
//#pragma gecko memory free(dE)
#pragma gecko memory freeobj(I,J,iN,iS,jW,jE,dN,dS,dW,dE,c)


//	free(c);
//#pragma gecko memory free(c)
	return 0;
}




void random_matrix(gecko_float I, int rows, int cols){

	srand(7);
	
	for( int i = 0 ; i < rows ; i++){
		for ( int j = 0 ; j < cols ; j++){
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

