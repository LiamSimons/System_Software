#include <stdio.h>

#define MEM_POOL_SIZE 10

typedef int size;

typedef enum {
    ALLOCATED, FREE
} mem_status;

typedef enum{
	HEADER, FOOTER
} mem_type;

typedef struct {
    size size;
    mem_status status;
} mem_chunk_header;

typedef unsigned char byte;
static byte mem_pool[MEM_POOL_SIZE];

int main(){
	void* addressWay;
	void* arrayWay;
	addressWay = mem_pool + (MEM_POOL_SIZE-1)*sizeof(byte) - sizeof(mem_chunk_header);
	arrayWay = &(mem_pool[(MEM_POOL_SIZE-1)-sizeof(mem_chunk_header)]);

	printf("ADDRESSWAY:\t%p\n", addressWay);
	printf("ARRAYWAY:\t%p\n", arrayWay);

	for(int i = 0; i < MEM_POOL_SIZE; i++){
		printf("%d %p\n", i, &mem_pool[i]);
	}

	printf("\nLAST ADDRESS IN ARRAY:\t%p\n", &mem_pool[0]);
	printf("\nLAST ADDRESS IN ARRAY:\t%p\n", &mem_pool[MEM_POOL_SIZE]);
	printf("\nLAST ADDRESS IN ARRAY:\t%p\n", &mem_pool[MEM_POOL_SIZE-1]);
	printf("SIZEOF MEM_CHUNK_HEADER:\t%ld\n", sizeof(mem_chunk_header));
	printf("LAST ADDRES - SIZE:\t%p\n", &mem_pool[599-sizeof(mem_chunk_header)]);
	return 0;
}