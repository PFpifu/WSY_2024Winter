#include<stdio.h>
#include<time.h>
#define MAXTIME 10000  // the function will run 10000 times.

clock_t start,stop;
double duration;

int main(){
	
	double x=1.0001,result;
	long int N,i,j;
	printf("Please input the exponent:");
	scanf("%d",&N);  //N is the exponent of the function.
	
	start = clock();
	
	for(i=1;i<=MAXTIME;i++){
		result = 1;  //set the result.
		for(j=1;j<=N;j++)
		    result *= x;  // this loop will compute X^N.
	}  //this loop will repeat the function for 10000 times.
	
	stop = clock();  //ticks = stop - start.
	
	duration = ((double)(stop-start))/CLK_TCK;  //compute the total time.
	printf("Ticks: %d\nTotal Time: %lf\n",stop-start,duration);
	
	return 0;
}
