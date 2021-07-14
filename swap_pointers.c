#include <stdio.h>
#include <stdlib.h>

void swap_pointers(int** p, int** q){
	int* dummy;
	dummy = *p;
	*p = *q;
	*q = dummy;
	return;
}

int main(){
	int a = 1;
	int b = 2;
	// for testing we use pointers to integers
	int *p = &a;
	int *q = &b;

	printf("Address of p = %p and q = %p\n", p, q);

	swap_pointers(&p, &q);

	printf("Address of p = %p and q = %p\n", p, q);
}