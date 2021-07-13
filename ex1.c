/*
	This is a very good example of code that will not compile:
		- the 'return &dummy' in date_struct returns an adress
		- this addres is on a local stack that will be deleted when date_struct is done
		- so the value returned will have no meaning after this 
	
	Fix this by:
		- allocate memory inside date_struct 
		- then return pointer to that allocated memory
		- make function to delete it, in which is a free of that memory
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    short day, month;
    unsigned year;
} date_t;

date_t* date_struct(int day, int month, int year){
	date_t dummy;					//date_t* dummy = (date_t*)malloc(sizeof(date_t));
	dummy./*->*/day = (short)day;		//-.
	dummy./*->*/month = (short)month;	//-.
	dummy./*->*/year = (unsigned)year;	//-.
	return &dummy; //-&
}

/*
void delete_date(date_t* date){
	free(date);
}
*/

int main(void){
	int day, month, year;							//make variables to put user input in
	date_t* date;									//make struct type to store this input
	printf("\nWhat is the day - month - year: ");	//ask input
	scanf("%d %d %d", &day, &month, &year);			//read input and put it in the right address
		//&day gives the address of the variable day so that's where to put the scanned input
	date = date_struct(day, month, year);
	printf("\nDate struct values: %d-%d-%d\n", date->day,
											   date->month,
											   date->year);
	//delete_date(date);
	return 0;
}