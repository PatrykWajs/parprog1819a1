#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//1) Δέσμευση πινάκων (+ έλεγχος ότι έγινε)
//2) Αρχικοποίηση (cache warmup)
// --> start time
//3) Φορτίο
// --> end time
//4) Έλεγχος
//5) Αποδέσμευση πινάκων

//gcc -Wall -O2 matmul-normal.c -o matmul-normal -DN=400

//#define N 400

void get_walltime(double *wtc) {
	struct timeval tp;
	gettimeofday(&tp,NULL);
	*wtc = (double)(tp.tv_sec+tp.tv_usec/1000000.0);
}

int main(){

//Δήλωση μεταβλητών

	int i, j, k;

	float *a,*b,*c;
	double ts,te,mflops;

	float *pa, *pb, *pc;
	float sum;

//1) Δέσμευση πινάκων (+ έλεγχος ότι έγινε)

	a = (float *)malloc(N*N*sizeof(float)); 
  	if (a==NULL) {
	    printf("alloc error!\n");
	    exit(1);
	}

	b = (float *)malloc(N*N*sizeof(float)); 
	if (b==NULL) {
	    printf("alloc error!\n");
	    free(a);
	    exit(1);
	}

	c = (float *)malloc(N*N*sizeof(float)); 
	if (b==NULL) {
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

// --> start time

	get_walltime(&ts);

//3) Φορτίο

	pc=c;

	for(k=0; k<N; k++) {
		pb = b;
		for(j=0; j<N; j++) {
			pa = a + k*N;
			sum = 0;
			for(i=0; i<N; i++) {
				sum = sum + (*pa) * (*pb);
				pa++;
				pb++;
			}
			*pc = sum;
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

	printf("Time = %f\nmflops = %f\n",te-ts,mflops);

//5) Αποδέσμευση πινάκων

	free(a);
	free(b);
	free(c);
	
	return 0;
}
