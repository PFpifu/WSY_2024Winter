#include<stdio.h>
#include<time.h>
#define MAXTIME 1000000  // the fuction will run 1000000 times.

clock_t start,stop;
double duration;
double POW(double x,long int N);// the recursive function.

int main(){
	
	double x=1.0001,result;
	long int N,i,j;
	printf("Please input the exponent:");
	scanf("%d",&N);  //N is the exponent of the function.
	
	start = clock();
	
	for(i=1;i<=MAXTIME;i++)
	    result = POW(x,N);  //this loop will repeat the function for 1000000 times.
	
	stop = clock();  //ticks = stop - start.
	
	duration = ((double)(stop-start))/CLK_TCK;  //compute the total time.
	printf("Ticks: %d\nTotal Time: %lf\n",stop-start,duration);
	
	return 0;
}

double POW(double x,long int N){
	if(N == 0)
	   return 1;
	if(N == 1)
	   return x;  //special cases are considered.
	if(N%2 == 0)
	   return POW(x*x, N/2); // if N is even,X^N = (x^2)^(N/2).
	else
	   return POW(x*x, N/2)*x;  //if N is odd,X^N = (X^2)^(N/2)*X.
}
