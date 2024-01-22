#include<stdio.h>
#include<time.h>
#define MAXTIME 100000  // the function will run 1e5 times.

clock_t start,stop;
double duration;
double Algo1(double x, long int N);  //function of algorithm 1.
double Algo2_R(double x, long int N);  //recursive function of algorithm 2.
double Algo2_I(double x, long int N);   //iterative function of algorithm 2.

int main(){
	
	double x=1.01,result;
	long int N,j;
	printf("Please input the exponent:");
	scanf("%d",&N);  //N is the exponent of the function.
	
	start = clock();
	for(j=1;j<=MAXTIME;j++)
	    result = Algo1(x,N);   //this loop will repeat the function for 1e5 times.
	stop = clock();
	
	duration = ((double)(stop-start))/CLK_TCK;  //compute the total time.
	printf("Ticks in algorithm 1: %d\nTotal Time in algorithm 1: %lf\n",stop-start,duration);
	
	start = clock();
	for(j=1;j<=MAXTIME*10;j++)
	    result = Algo2_R(x,N);   //this loop will repeat the function for 1e6 times.
	stop = clock();
	
	duration = ((double)(stop-start))/CLK_TCK;  //compute the total time.
	printf("Ticks in algorithm 2_recursive: %d\nTotal Time in algorithm 2_recursive: %lf\n",stop-start,duration);
	
	start = clock();
	for(j=1;j<=MAXTIME*10;j++)
	    result = Algo2_I(x,N);   //this loop will repeat the function for 1e6 times.
	stop = clock();
	
	duration = ((double)(stop-start))/CLK_TCK;  //compute the total time.
	printf("Ticks in algorithm 2_iterative: %d\nTotal Time in algorithm 2_iterative: %lf\n",stop-start,duration);
	
	return 0;
	
}

double Algo1(double x, long int N){
	double result1;
	long int i;
	result1 = 1;
	for(i=1;i<=N;i++)
	    result1 *= x;  //this loop will compute X^N.
	return result1;
}

double Algo2_R(double x, long int N){
	if(N == 0)
	   return 1;
	if(N == 1)
	   return x;  //special cases are considered.
	if(N%2 == 0)
	   return Algo2_R(x*x, N/2); // if N is even,X^N = (x^2)^(N/2).
	else
	   return Algo2_R(x*x, N/2)*x;  //if N is odd,X^N = (X^2)^(N/2)*X.
}

double Algo2_I(double x, long int N){
	double result2,temp;
	long int i;
	result2 = 1;
    if(N==0) return result2; //special cases are considered.
    temp = x;
    for(i=N;i>=1;i=i/2){
    		if(i%2) result2 *= temp; //if i is odd,X^i = X^(i/2)*X^(i/2)*X.
    		temp *= temp;// if i is even,x^i = X^(1/2)*X^(i/2).
    }
    return result2;
}
