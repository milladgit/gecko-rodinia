#include <stdio.h>

extern char *exec_loc;
extern char *exec_policy_chosen;

void lud_openacc(gecko_float a, int size)
{
     int i,j,k;
     float sum;
//     #pragma acc data copy(a[0:size*size])
     for (i=0; i <size; i++){

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen) variable_list_internal(a)
         #pragma acc parallel loop
         for (j=i; j <size; j++){
             sum=a[i*size+j];
             #pragma acc loop seq
             for (k=0; k<i; k++) sum -= a[i*size+k]*a[k*size+j];
             a[i*size+j]=sum;
         }
#pragma gecko region end

//#pragma gecko region pause at(exec_loc)


	     
#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen) variable_list_internal(a)
         #pragma acc parallel loop
         for (j=i+1;j<size; j++){
             sum=a[j*size+i];
             #pragma acc loop seq
             for (k=0; k<i; k++) sum -=a[j*size+k]*a[k*size+i];
             a[j*size+i]=sum/a[i*size+i];
         }
#pragma gecko region end

//#pragma gecko region pause at(exec_loc)
     }
#pragma gecko region pause at(exec_loc)
	
}
