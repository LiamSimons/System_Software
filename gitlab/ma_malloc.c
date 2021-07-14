/**
 * \author Liam Simons
 */

#include "ma_malloc.h"
#include <stdio.h>

#define MEM_POOL_SIZE 600   //in bytes
#define bytes_row (int)10
#define ad_col_width (int)2*sizeof(void*)
#define byte_col_width (int)bytes_row*3
#define col_separator " "

typedef unsigned char byte;

//! An enum of statuses that can be used in the header
typedef enum {
    ALLOCATED, FREE
} mem_status;

typedef enum{
	HEADER, FOOTER
}mem_type;

//! Every item in the pool will have at least a header with size and status of the following data
typedef struct {
    size size;
    mem_status status;
    //mem_type type;
} mem_chunk_header;


static byte mem_pool[MEM_POOL_SIZE]; 
const int header_size = sizeof(mem_chunk_header);

/**
 * Allocates array of bytes (memory pool) and initializes the memory allocator.
 * If some bytes have been used after calling ma_malloc(size), calling to ma_init() will result in clearing up the memory pool.
 */
void ma_init() {

    /*printf("mem_status\n"); //4
    printf("mem_chunk_header\n"); //8
    printf("pointer to int\n"); //8
    printf("pointer to mem_chunk_header\n"); //8 different 8 as mem_chunk_header*/

    mem_chunk_header * header;						//first header of the mem_pool
    mem_chunk_header * footer;						
    header =(mem_chunk_header*) mem_pool;			//SAME as &mem_pool[0] - Cast at mem_chunk_header and starts at mem_pool
    //printf("\nfooterstart: %i\nmem_pool_size: %i\nheader_size: %i\n", MEM_POOL_SIZE - header_size, MEM_POOL_SIZE, header_size);
    footer = (mem_chunk_header*) &mem_pool[MEM_POOL_SIZE - header_size];
    //printf("   Header pointer: %p and footer pointer: %p\n", header, footer);

    header->status = FREE;
    header->size = MEM_POOL_SIZE - 2*header_size;
    //header->type = HEADER;

    footer->status = FREE;
    footer->size = header->size;
    //printf("Header size = %i\n", header->size);
    //footer->type = FOOTER;

    //printf("idk\n");
    //printf("%p\n", first);
}

/**
 * Requesting for the tsize bytes from memory pool.
 * If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
 */
void *ma_malloc(size tsize) {

	//check if there is enough free memory
	/*mem_chunk_header * first;
	first = (mem_chunk_header*) mem_pool;
	mem_chunk_header * newheader;
	newheader = first + sizeof(mem_chunk_header) + tsize;
	newheader->status = FREE;*/
	//newheader->size = //calculate size

	mem_chunk_header* header = NULL;
	size s = 0; 
	void* pointer = NULL;
	//enum status;


	while(s<MEM_POOL_SIZE)
	{
		//*header = *(mem_chunk_header*)&mem_pool[s];
		header = (mem_chunk_header*) &mem_pool[s];
		//printf("headerdummy = %p\n", header);					//checking
		if(header->status == FREE)
		{
			if(header->size >= tsize)
			{
				pointer = header;//then allocate the memory => het is FREE van status en groot genoeg in size
				break;
			}
			else{s = s + header->size + 2*header_size;}
		}
		else{s = s + header->size + 2*header_size;}
	}
	
	if(pointer != NULL)
	{
		int newSize = header->size - (tsize + 2*header_size);
		header->size = tsize;
		header->status = ALLOCATED;
		//header->type = HEADER;

		mem_chunk_header* footer = (mem_chunk_header*)(pointer + tsize + header_size); //??
		footer->status = ALLOCATED;
		footer->size = tsize;
		//footer->type = FOOTER;

		mem_chunk_header* new_header = (mem_chunk_header*)(pointer + tsize + 2*header_size);
		//printf("new_header = %p\n", new_header);				//checking
		new_header->status = FREE;
		new_header->size = newSize;
		//new_header->type = HEADER;
		//printf("ma_malloc : pointer to header = %p   |   footer = %p   |   new header = %p\n", pointer, footer, new_header);
	}
	//printf("STATUSSSS %s\n", (char*)header->status);
	/*if(header->status == FREE) printf("   Status = %i NOT ALLOCATED\n\n", tsize);
	else if(header->status == ALLOCATED)printf("   Status = %i ALLOCATED\n\n", tsize);
	else printf("   ERROR ma_malloc\n\n");*/
	return pointer;
}

/**
 * Releasing the bytes in memory pool which was hold by ptr, meaning makes those bytes available for other uses.
 * Implement also the coalescing behavior.
 */
void ma_free(void *ptr) {

    //TODO: add your code here
	mem_chunk_header* header = (mem_chunk_header*)(ptr);
	//printf("ma_free : header = %p\n", header);
	//printf("ma_free : header_size = %i\n", header_size);
	mem_chunk_header* footer = (mem_chunk_header*)(ptr + header->size + header_size);
	//printf("ma_free : footer = %p\n", footer);
	/*if(header->status == FREE) printf("Status = FREE\n");
	else if(header->status == ALLOCATED)printf("Status = ALLOCATED\n");
	else printf("ERROR\n");*/
	
	header->status = FREE;
	footer->status = FREE;

	//coalescing:
	mem_chunk_header* prev = header - 1;
	mem_chunk_header* next = footer + 1;
	//printf("ma_free : prev = %p   |   next = %p\n", prev, next);

	if(prev->status == FREE)
	{
		size prevSize = prev->size;
		//printf("\nPREVIOUS:\nma_free : prev size = %i\n", prevSize);
		mem_chunk_header* newHeader = (mem_chunk_header*)((byte*)prev - prevSize - header_size);
		//printf("ma_free : prev header = %p\n", newHeader);
		newHeader->size = prevSize + 2*header_size + header->size;
		//printf("ma_free : new header size = %i\n", newHeader->size);
		footer->size = newHeader->size;
		//printf("ma_free : footer size = %i\n", footer->size);
		header = newHeader;
		//printf("ma_free : new header = %p\n", header);
	}
	/*else if(prev->status == ALLOCATED){
		printf("Prev status = ALLOCATED\n");
	}
	else{
		printf("ER IS WEERAL IETS FOUT GVD\n");
	}*/

	if(next->status == FREE)
	{
		size nextSize = next->size;
		mem_chunk_header* newFooter = (mem_chunk_header*)((byte*)next + nextSize + header_size);
		newFooter->size = nextSize + 2*header_size + header->size;
		header->size = newFooter->size;
		// printf("\nPREVIOUS:\nma_free : next size = %i\n", nextSize);
		// printf("ma_free : new footer = %p\n", newFooter);
		// printf("ma_free : new footer size = %i\n", newFooter->size);
		// printf("ma_free : footer size = %i\n", footer->size);
		// printf("ma_free : new header = %p\n", header);
		//rintf("ma_free newsize = %i\n", newSize);
	}
	/*else if (next->status == ALLOCATED){
		printf("Next status = ALLOCATED\n");
	}
	else{
		printf("ER IS WEERAL IETS FOUT GVD\n");
	}*/
	//printf("ma_free : header = %p\nma_free : size = %i\n", header, header->size);
		
}

/**
 * This function is only for debugging. It prints out the entire memory pool.
 * Print info on headers and content of all elements in your pool
 */
void ma_print(void) {

    //TODO: add your code here
	/*byte *ptr = mem_pool;
  	long size = MEM_POOL_SIZE;

  	int i;
  	// int increment = 1;
  	// if(size < 0) {
  	// 	increment = -1;
  	// }

  	// print table header
  	//printf("\n%-*s", ad_col_width, "Address");
  	printf("%s", col_separator);
  	printf("%-*s", byte_col_width, "Bytes");
  	printf("%s", col_separator);
  	printf("%-*s\n", bytes_row, "Chars");

  	for(i=0; i<ad_col_width; i++){
  		printf("-");
  		printf("%s", col_separator);
  	}
  	i = 0;

  	for(i=0; i<byte_col_width; i++){
  		printf("-");
  		printf("%s", col_separator);
  	}
  	i = 0;

  	for(i=0; i<bytes_row; i++){
  		printf("-");
  		printf("\n");
  	}
  	i = 0;

  	printf("%p\n", ptr);
  	printf("%ld\n", size);*/
}
 
/*void checkStatus(void *ptr){
	mem_chunk_header* header = ptr;
	if(header->status == FREE){
		printf("   FREE\n");
	}
	else if(header->status == ALLOCATED){
		printf("   ALLOCATED\n");
	}
	else{
		printf("   ERROR: The function checkStatus() doesn't work!\n");
		printf("   %p\n", ptr);
		//printf("%s", (char*)header->status);
	}
	printf("   Size = %i\n", header->size);
}*/
  
