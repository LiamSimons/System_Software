#include <stdio.h>
#include <stdlib.h>

typedef struct {
    short day, month;
    unsigned year;
} date_t;

void date_struct(int day, int month, int year, date_t* date){
	date_t dummy;					
	dummy.day = (short)day;		
	dummy.month = (short)month;
	dummy.year = (unsigned)year;
	*date = dummy; 					//date = &dummy;	//code in ex1 that doesn't work
	//this works 
}

int main(void){
	int day, month, year;							
	date_t date;								
	printf("\nWhat is the day - month - year: ");	
	scanf("%d %d %d", &day, &month, &year);			
	date_struct(day, month, year, &date);
	printf("\nDate struct values: %d-%d-%d\n", date.day,
											   date.month,
											   date.year);
	return 0;
}