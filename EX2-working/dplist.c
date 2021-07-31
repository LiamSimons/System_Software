/**
 * \author Liam Simons
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

//#define DEBUG

/*
 * definition of error codes
 */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1   //error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2  //error due to a list operation applied on a NULL list

#ifdef DEBUG
#define DEBUG_PRINTF(...) 									                                        \
        do {											                                            \
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	    \
            fprintf(stderr,__VA_ARGS__);								                            \
            fflush(stderr);                                                                         \
                } while(0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition, err_code)                         \
    do {                                                                \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");      \
            assert(!(condition));                                       \
        } while(0)


/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};


// FUNCTIONS -------------------------------------------------
dplist_t *dpl_create(){
	dplist_t* list;
	list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
	list->head = NULL;
	return list;
}

void dpl_free(dplist_t **list){
	// Remove all elements
	if(list == NULL) return;
	if(*list == NULL) return;
	if((*list)->head != NULL){
		dplist_node_t* dummy_node;
		//dplist_node_t* temporary_node;
		dummy_node = (*list)->head;
		while(dummy_node){
			dplist_node_t* temporary_node = dummy_node->next; // declaring before while gives incompatible type error
			free(dummy_node);
			dummy_node = temporary_node;
		}
	}
	// Remove list
	free(*list);
	*list = NULL;
}
int dpl_size(dplist_t *list){
	if(list == NULL) return -1;
	if(list->head == NULL) return 0;
	int size = 0;
	dplist_node_t* dummy = list->head;
	while(dummy){
		size ++;
		dummy = dummy->next;
	}
	return size;
}
dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index){
	dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));
    DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
	return list;
}
dplist_t *dpl_remove_at_index(dplist_t *list, int index){
	if(list == NULL) return NULL;
	dplist_node_t* dummy = dpl_get_reference_at_index(list, index);
	if(dummy == NULL) return list;
	if(index <= 0){
		list->head = dummy->next;
	} 
	else if(index >= dpl_size(list) - 1){
		dplist_node_t* prev = dummy->prev;
		prev->next = NULL;
	}else{
		dplist_node_t* prev = dummy->prev;
		dplist_node_t* next = dummy->next;
		prev->next = next;
		next->prev = prev;
	}
	free(dummy);
	return list;
}
dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index){
	//DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
	if(list == NULL) return NULL;
	if(list->head == NULL) return NULL;
	dplist_node_t* dummy;
	int count = 0;
	dummy = list->head;
	while(dummy->next != NULL && count < index){
		//if(count >= index) return dummy;
		count ++;
		dummy = dummy->next;
	}
	return dummy;
}
void* dpl_get_element_at_index(dplist_t *list, int index){
	if(list == NULL) return NULL;
	if(list->head == NULL) return NULL;
	return (void*)dpl_get_reference_at_index(list, index)->element;
}
int dpl_get_index_of_element(dplist_t *list, element_t element){
	if(list == NULL) return -1;
	if(list->head == NULL) return -1;
	dplist_node_t* dummy;
	dummy = list->head;
	if(dummy->element == element) return 0;
	int index = 0;
	while(dummy->next != NULL){
		if(dummy->element == element) return index;
		dummy = dummy->next;
		index ++;
	}
	if(dummy->element == element) return index;
	return -1;
}
