#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FREQUENCY	1	//SECONDS
#define MIN_TEMP	-1	//°C
#define MAX_TEMP	3	//°C

int main(){
	time_t t;												//used as seed for the srand function
	
	srand((unsigned) time(&t));								//put the current time in t - use this as seed for the rand generator
															//no use - requires further research, can be commented out now
	int temp_range = MAX_TEMP - MIN_TEMP;					//calculate the range of values the srand can have
	
	while(1){												
		printf("%d\n", (rand()%(temp_range+1) + MIN_TEMP));	//print random values in the range specified
		sleep(1/FREQUENCY);									//sleep for the inverse of the frequency = period
		fflush(stdout);										//this is a buffered function and this prevents buildup in the buffer
	}
}