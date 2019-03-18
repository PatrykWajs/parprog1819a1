#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pmmintrin.h>

//1) Δέσμευση πινάκων (+ έλεγχος ότι έγινε)
//2) Αρχικοποίηση (cache warmup)
// --> start time
//3) Φορτίο
// --> end time
//4) Έλεγχος
//5) Αποδέσμευση πινάκων

//gcc -Wall -O2 matmul-sse.c -o matmul-sse -DN=400

//#define N 400

void get_walltime(double *wtc) {
	struct timeval tp;
	gettimeofday(&tp,NULL);
	*wtc = (double)(tp.tv_sec+tp.tv_usec/1000000.0);
}

int main(){

//Δήλωση μεταβλητών

	int i, j, k;

	float *a,*b,*c,*zero;
	double ts,te,mflops;
	
	__m128 *va, *vb;

	
//1) Δέσμευση πινάκων (+ έλεγχος ότι έγινε)
	
	i=0;
	i = posix_memalign((void**)&a, 16, N*N*sizeof(float));
    		if(a!=0) {
			printf("alloc error!\n");
        		exit(1);
    		}
	j=0;
    	j = posix_memalign((void**)&b, 16, N*N*sizeof(float));
    		if(b!=0) {
			printf("alloc error!\n");
       			free(a);
       			exit(1);
    		}
	k=0;
    	k = posix_memalign((void**)&c, 16, N*N*sizeof(float));
    		if(c!=0) {
			printf("alloc error!\n");
        		free(a);
        		free(b);
        		exit(1);
   		}

//2) Αρχικοποίηση (cache warmup)

	for(i=0; i<N*N; i++) {
		a[i] = 2.0;
		b[i] = 3.0;
		c[i] = 20.0;
	}

	for (i=0; i<4; i++) {
        zero[i] = 0.0;
	}

	__m128 *sum = (__m128*) zero;
	__m128 tmp;

	float *pc = c;

// --> start time

	get_walltime(&ts);

//3) Φορτίο
	

	for( k = 0; k < N; k++) 
    	{	
		vb =(__m128*) b;
        	for(j = 0; j < N; j++) 
        	{
            	va = (__m128*) (a + k*N);
		*sum = _mm_set_ps(0, 0, 0, 0);

            	for( i = 0; i < N; i+=4) 
		    {   
		        *sum = _mm_add_ps(*sum, _mm_mul_ps(*va, *vb));
			va++, vb++;
		    }

		    tmp = _mm_hadd_ps(*sum, *sum);
            	    *sum = _mm_hadd_ps(tmp, tmp);
            	    *pc = zero[0];
		    pc++;
		 }
	 }

// --> end time

	get_walltime(&te);

//4) Έλεγχος

	for(i= 0; i<N;i++)
       	{
           for(j=0;j<N;j++)
           {
               
               if(c[j*N+i] != 6*N)
               {
                   printf("Error");
                   exit(3);
               }
	    }
         }
           

//Υπολογισμος και εκτυπωση χρονου

	mflops = (N* N) / ((te - ts) * 1e6);

	printf("Time = %f\nmflops =%f\n",te-ts,mflops);


//5) Αποδέσμευση πινάκων

	free(a);
	free(b);
	free(c);
	
	return 0;
}
