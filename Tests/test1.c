#include <stdio.h>

typedef unsigned char byte;
static byte mem_pool[600];

int main(){
	printf("\nMEM_POOL: \t\t%p\n", mem_pool);
	printf("MEM_POOL[0] ADDRESS: \t%p\n", &mem_pool[0]);
	return 0;
}