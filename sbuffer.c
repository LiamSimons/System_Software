/**
 * \author Liam Simons
 */

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
	int stop;					// ADDED: when this is read they will know to stop
	int reader_datamgr_flag;	// ADDED: will be set to 1 when corresponding thread has read it
	int reader_sensor_db_flag;	// ADDED: will be set to 1 when corresponding thread has read it
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
};

// Extra variables
pthread_mutex_t lock_buffer = PTHREAD_MUTEX_INITIALIZER;	// ADDED: make buffer thread safe - link in main.c
pthread_cond_t data_inserted = PTHREAD_COND_INITIALIZER;	// ADDED: wait for insertion of data to continue

int sbuffer_init(sbuffer_t **buffer) {
	pthread_mutex_lock(&lock_buffer);		// ADDED
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL){
		pthread_mutex_unlock(&lock_buffer);	// ADDED
		return SBUFFER_FAILURE;
	}
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
	pthread_mutex_unlock(&lock_buffer);		// ADDED
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
	pthread_mutex_lock(&lock_buffer);		// ADDED
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
		pthread_mutex_unlock(&lock_buffer);	// ADDED
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
	pthread_mutex_unlock(&lock_buffer);		// ADDED
	pthread_mutex_destroy(&lock_buffer);	// ADDED
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer/*, sensor_data_t *data*/) {
	// ADDED: Will be called from sbuffer_read so is already in thread lock
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;
    //*data = buffer->head->data; // ADDED: this together with the data argument is not necessary when using seperate read function
	if(buffer->head->reader_datamgr_flag && buffer->head->reader_sensor_db_flag){	// ADDED: only remove when both threads have read it
		dummy = buffer->head;
		printf("\t> SBUFFER: Removing node [id = %d; value = %f; ts = %ld]\n", dummy->data.id, dummy->data.value, dummy->data.ts); // ADDED
		if (buffer->head == buffer->tail) // buffer has only one node
		{
			buffer->head = buffer->tail = NULL;
		} else  // buffer has many nodes empty
		{
			buffer->head = buffer->head->next;
		}
		free(dummy);
	}
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data, int stop_and_go) {
	pthread_mutex_lock(&lock_buffer);	// ADDED
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
	dummy->stop = stop_and_go;	// ADDED: SBUFFER_STOP | SBUFFER_GO
	dummy->reader_datamgr_flag = 0;
	dummy->reader_sensor_db_flag = 0;
	printf("\t> SBUFFER: Inserting node [id = %d; value = %f; ts = %ld]\n", dummy->data.id, dummy->data.value, dummy->data.ts);	// ADDED
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
	pthread_cond_signal(&data_inserted);
	pthread_mutex_unlock(&lock_buffer);	// ADDED
    return SBUFFER_SUCCESS;
}
