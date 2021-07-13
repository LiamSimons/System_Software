#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FREQUENCY	1	//SECONDS
#define MIN_TEMP	-1	//°C
#define MAX_TEMP	3	//°C

int main(){
	time_t t;
	
	srand((unsigned) time(&t));		//put the current time in t - use this as seed for the rand generator
	int temp_range = MAX_TEMP - MIN_TEMP;
	
	while(1){
		printf("%d\n", (rand()%(temp_range+1) + MIN_TEMP));
		sleep(FREQUENCY);
		fflush(stdout);
	}
}