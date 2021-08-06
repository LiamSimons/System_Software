#include <stdio.h>
#include <poll.h>
#include "connmgr.h"

// CALLBACK FUNCTIONS //
void* element_copy(void* element){
	void* copy = (void*)malloc(sizeof(void));
	return copy;
}
void element_free(void** element){
	free(*element);
}
int element_compare(void* x, void* y){
	return 0;
}

dplist_t* list;

// CONNMGR FUNCTIONALITY //
void connmgr_listen(int port_number){
	list = dpl_create(element_copy, element_free, element_compare);
	//
		/**
		*	Now test the poll function
		*/
	//
	printf("CONN LISTEN\n");
}

void connmgr_free(){
	dpl_free(&list, true);
	printf("CONN FREE\n");
}