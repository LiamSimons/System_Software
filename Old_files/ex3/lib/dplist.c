/**
 * \author Liam Simons
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 

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
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {

    if (list == NULL){
        return;
    }
    else if(*list == NULL){
        // free(*list);
        return;
    }
    else{
        int size = dpl_size(*list)-1;
        while(size>=0){
            *list = dpl_remove_at_index(*list, size, free_element);
            size = size -1;
        }
        dplist_t* temp = *list;
        free(temp);
        *list = NULL;
    }
    // if(list == NULL){
    //     return;
    // }
    // else if(*list == NULL){
    //     free(*list);
    //     return;
    // }
    // else if((*list)->head == NULL){
    //     free(*list);
    //     *list = NULL;
    //     return;
    // }
    // else{
    // //     int size = dpl_size(*list)-1;
    // //     while(size>=0){
    // //         *list = dpl_remove_at_index(*list, size, free_element);
    // //         size --;
    // //     }
    // // }
    //     dplist_t *temp = *list;
    //     dplist_node_t *node = temp->head;
    //     //clear the head
    //     // dplist_node_t *temp_node = node->next;
    //     // free(node);
    //     // node = temp_node;
    //     //clear nodes
    //     if (!free_element){
    //         while(node){
    //             dplist_node_t *temp = node->next;
    //             free(node);
    //             node = temp;
    //         }
    //     }
    //     else{
    //         dplist_node_t *temp = node->next;
    //         free(node);
    //         node = temp;
    //         while(node){
    //             dplist_node_t *temp = node->next;
    //             (*list)->element_free(&(node->element));
    //             free(node);
    //             node = temp;
    //         }
    //     }
    //     free(*list);
    //     *list = NULL;
    // }
    return;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;
    if (element == NULL) return list;

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
    if(insert_copy){
        list_node->element = list->element_copy(element);
    }
    else{
        list_node->element = element;
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    if(list == NULL) return NULL;
    else if (list->head == NULL) return list;
    else{
        dplist_node_t *node = dpl_get_reference_at_index(list, index);
        if(free_element) list->element_free(&(node->element));
        if(node->prev == NULL){
            if(node->next == NULL){
                list->head = NULL;
                free(node);
            }
            else{
                list->head = node->next;
                node->next->prev = NULL;
                free(node);
            }
        }
        else{
            if(node->next == NULL){
                node->prev->next = NULL;
                free(node);
            }
            else{
                node->prev->next = node->next;
                node->next->prev = node->prev;
                free(node);
            }
        }
    }
    return list;
    // if(list != NULL){
    //     dplist_node_t *node = dpl_get_reference_at_index(list, index);
    //     if(index <= 0){
    //         //dplist_t *list = node->prev;
    //         list->head = node->next;
    //     }
    //     else if(index >= dpl_size(list)){
    //         dplist_node_t *prev = node->prev;
    //         prev->next = NULL;
    //     }
    //     else{
    //         dplist_node_t *prev = node->prev;
    //         dplist_node_t *next = node->next;
    //         prev->next = node->next;
    //         next->prev = node->prev;
    //     }
    //     if(free_element){
    //         list->element_free(&(node->element));
    //     }
    //     free(node->element);
    //     free(node);
    //     //TODO: add your code here
    //     return list;
    // }
    // else{
    //     return NULL;
    // }

}

int dpl_size(dplist_t *list) {
    
    if(list == NULL) return -1;
    if(list->head == NULL) return 0;
    int size = 0;
    dplist_node_t *node = list->head;
    while(node){
        node = node->next;
        size++;
    }
    return size;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    if(list != NULL){
        dplist_node_t *node = dpl_get_reference_at_index(list, index);
        return node->element;
    }
    else{
        return 0;
    }
}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    if(list == NULL){
        return -1;
    }
    else{
        if(dpl_size(list) == 0){
            return -1;
        }
        else{
            dplist_node_t *node = list->head;
            int index = -1;
            int found = 0;
            while(found == 0 && node)
            {
                if(list->element_compare(node->element, element) == 0){
                    found = 1;
                }
                index++;
                node = node->next;   
            }
            if(found == 1){
                return index;
            }
            else{
                return -1;
            }
            // return index;
            // if(loop == 0){
            //     return index;
            // }
            // else{
            //     return -1;
            // }
        }
    }
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    if(list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    int count;
    dplist_node_t *dummy;
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++) {
        if (count >= index) return dummy;
    }
    return dummy;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    if(list == NULL || dpl_size(list) == 0 || reference == NULL) return NULL;
    else{
        dplist_node_t *node = list->head;
        while(node){
            if(node == reference){
                return node->element;
            }
            else{
                node = node->next;
            }
        }
        return NULL;
    }
}


