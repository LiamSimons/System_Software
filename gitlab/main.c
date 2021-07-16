/**
 * \author Liam Simons
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "ma_malloc.h"

//You need to make a sketch of your memory pool at different points in your program indicated by a number
//Which parts are free, which are allocated?
//Where are your headers/footers - how many bytes do they take? What is stored in it?
//Where are the pointers returned by ma_malloc?

int main(int argc, char *argv[]) {
    char *ptr[] = {NULL, NULL, NULL};
//sketch 1
    ma_init();
        //printf("\nTest 0: ma_init works\n");

//sketch 2
    assert(NULL == ma_malloc(600)); //->should return NULL
    assert(NULL != ma_malloc(200)); //->should not return NULL;
        //printf("Test 1: succeeded //ma_malloc 600 too large - 200 ok\n");
    ma_init();
    ptr[0] = ma_malloc(400); //->should not return NULL (ma_init() clears all)
        //printf("Test 2: succeeded //ptr[0] = ma_malloc(400) returned pointer:\n   %p\n\n", ptr[0]);
    //printf("%p \n", ptr);
//sketch 3
    assert(ptr[0] != NULL);
    // for debugging:
    // memset(ptr[0], 'a', 400); //-> makes this area easy to find when using GDB
        //printf("Test 3: check status 1 //ma_free freed ptr[0] status:\n");
        //checkStatus(ptr[0]);
    ma_free(ptr[0]);

    
        //printf("Test 3: check status 2 //ma_free freed ptr[0] status:\n");
        //checkStatus(ptr[0]);
        //printf("            ->  correct is first allocated then free!\n\n");

    ptr[0] = ma_malloc(200); //-> should not return NULL
    assert(ptr[0] != NULL);
        //printf("Test 4: ptr[0] set:\n");
        //checkStatus(ptr[0]);
        //printf("\n");
    //ma_print();
    // for debugging:
    // memset(ptr[0], 'b', 200); //-> makes this area easy to find when using GDB

    ptr[1] = ma_malloc(100); //-> should not return NULL
        //printf("   Pointer ptr[1]:\n%p\n\n", ptr[1]);
    assert(ptr[1] != NULL);
        //printf("Test 5: ptr[1] set:\n");
        //checkStatus(ptr[1]);
    // for debugging:
    // memset(ptr[1], 'b', 100); //-> makes this area easy to find when using GDB or memdump-tool

    ptr[2] = ma_malloc(100); //-> should not return NULL
//sketch 4
    assert(ptr[2] != NULL);
        //printf("Test 6: ptr[2] set:\n");
        //checkStatus(ptr[2]);
    // for debugging:
    // memset(ptr[2], 'c', 100); //-> makes this area easy to find when using GDB or memdump-tool

    ma_free(ptr[1]);
//sketch 5
    ma_free(ptr[2]); //-> mind the sequence, first free 1 than 2
//sketch 6
    assert(NULL != ma_malloc(250)); //-> should not return NULL (test for coalescing)
	printf("MAIN DONE\n");
    return 0;
}
