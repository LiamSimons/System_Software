#include <stdio.h>
#include <stdlib.h>

int main()
{
	//printf("test\n");
	printf("Void:\n");
	printf("     ");
	printf("%lu\n", sizeof(void));
	printf("Normal int:\n");
	printf("     ");
	printf("%lu\n", sizeof(int));
	printf("Short int:\n");
	printf("     ");
	printf("%lu\n", sizeof(short int));
	printf("Long int:\n");
	printf("     ");
	printf("%lu\n", sizeof(long int));
	printf("Float:\n");
	printf("     ");
	printf("%lu\n", sizeof(float));
	printf("Long double:\n");
	printf("     ");
	printf("%lu\n", sizeof(long double));
	printf("Double:\n");
	printf("     ");
	printf("%lu\n", sizeof(double));
	printf("Pointer:\n");
	printf("     ");
	printf("%lu\n", sizeof(void*));
	return 0;
}