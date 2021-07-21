/**
*\author Liam Simons
*/
//------------------------ INCLUDES AND DEFINES -------------------------//

#include "ma_malloc.h"
#include <stdio.h>

#define MEM_POOL_SIZE 600		// 600 bytes for full memory pool
// defines that were already in the code
#define bytes_row (int)10
#define ad_col_width (int)2*sizeof(void*)
#define byte_col_width (int)bytes_row*3
#define col_separator " "

//----------------------- VARIABLES -------------------------------------//

typedef unsigned char byte;		// easy to use byte variable

//! An enum of statuses that can be used in the header
typedef enum {
    ALLOCATED, FREE
} mem_status;

typedef enum{
	HEADER, FOOTER
} mem_type;

//! Every item in the pool will have at least a header with size and status of the following data
typedef struct {
    size size;
    mem_status status;
} mem_chunk_header;


static byte mem_pool[MEM_POOL_SIZE]; 	// mem_pool is pointer to first element
const int header_size = sizeof(mem_chunk_header);


//----------------------- FUNCTIONS -------------------------------------//
void ma_init(){
	// Header and Footer pointer assignment
	mem_chunk_header* header;
	mem_chunk_header* footer;
	header = (mem_chunk_header*) mem_pool;
	footer = (mem_chunk_header*) &(mem_pool[MEM_POOL_SIZE - header_size]);

	// Set status and size of header and pointer
	header->status = FREE;
	header->size = MEM_POOL_SIZE - 2*header_size;
	footer->status = FREE;
	footer->size = header->size;
}

void *ma_malloc(size tsize){
	void* free_chunk_header = NULL;	
	mem_chunk_header* dummy_header = NULL;
	size dummy_index = 0;

	// Find a free memory chunk 
	while(dummy_index < MEM_POOL_SIZE){
		dummy_header = (mem_chunk_header*) &mem_pool[dummy_index];
		if(dummy_header->status == FREE){
			if(dummy_header->size >= tsize){
				free_chunk_header = dummy_header;
				break;
			}
			else dummy_index += dummy_header->size + 2*header_size;
		}
		else dummy_index += dummy_header->size + 2*header_size;
	}
	// Allocate the memory chunk
	if(free_chunk_header != NULL){
		size new_free_size = dummy_header->size - (tsize + 2*header_size);
		
		dummy_header->status = ALLOCATED;
		dummy_header->size = tsize;

		mem_chunk_header* alloc_footer = (mem_chunk_header*)((byte*)free_chunk_header + tsize + header_size);
		alloc_footer->status = ALLOCATED;
		alloc_footer->size = tsize;

		mem_chunk_header* new_free_header = (mem_chunk_header*)((byte*)free_chunk_header + tsize + 2*header_size);
		new_free_header->status = FREE;
		new_free_header->size = new_free_size;
	}
	return free_chunk_header;
}

void ma_free(void *ptr){
	mem_chunk_header* header = (mem_chunk_header*) ptr;
	mem_chunk_header* footer = (mem_chunk_header*)(ptr + header->size + header_size);
	header->status = FREE;
	footer->status = FREE;

	//Coalescing
	mem_chunk_header* prev = header - 1;
	mem_chunk_header* next = footer + 1;
	//Check header not first and footer not last
	if(header != (mem_chunk_header*)mem_pool){
		if(prev->status == FREE){
			size prev_size = prev->size;
			mem_chunk_header* prev_header = (mem_chunk_header*)((byte*)header - prev_size - 2*header_size);
			prev_header->size = header->size + prev_size + 2*header_size;
			footer->size = prev_header->size;
			header = prev_header;	//needed for when next chunk is free
		}		
	}
	if(footer != (mem_chunk_header*)&(mem_pool[MEM_POOL_SIZE - header_size])){
		if(next->status == FREE){
			size next_size = next->size;
			mem_chunk_header* next_footer = (mem_chunk_header*)((byte*)footer + next_size + 2*header_size); //this byte pointer cast made it work idk why
			header->size = header->size + next_size + 2*header_size;
			next_footer->size = header->size;
		}
	}
}

void ma_print(void){

}