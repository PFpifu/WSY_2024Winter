#include<stdio.h>
#include<time.h>
#define MAXTIME 1000000  // the fuction will run 1000000 times.

clock_t start,stop;
double duration;

int main(){
	
	double x=1.0001,result,temp;
	long int N,i,j;
	printf("Please input the exponent:");
	scanf("%d",&N);  //N is the exponent of the function.
	
	start = clock();
	
    for(j=1;j<MAXTIME;j++){
    	result = 1;
    	if(N==0) continue; //special cases are considered.
    	temp = x;
    	for(i=N;i>1;i=i/2){
    		if(i%2) result *= temp; //if i is odd,X^i = X^(i/2)*X^(i/2)*X.
    		temp *= temp;// if i is even,x^i = X^(1/2)*X^(i/2).
    	}
    }  //this loop will repeat the function for 1000000 times.
    
    stop = clock();
    
    duration = ((double)(stop-start))/CLK_TCK;  //compute the total time.
    printf("Ticks: %d\nTotal Time: %lf\n",stop-start,duration);
    
    return 0;
    
}
