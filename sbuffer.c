/**
 * \author Liam Simons
 */

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"

#ifdef DEBUG
# define DEBUG_PRINT(...) 				\
			do{							\
				printf("DEBUG: ");		\
				printf(__VA_ARGS__);	\
			}while(0)				
#else
# define DEBUG_PRINT(...) do {} while (0)
#endif

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
	
	if((buffer->head->reader_datamgr_flag && buffer->head->reader_sensor_db_flag) || buffer->head->stop == SBUFFER_STOP){	// ADDED: only remove when both threads have read it
		dummy = buffer->head;
		if(dummy->stop == SBUFFER_GO) printf("\t> SBUFFER: Removing node [id = %d; value = %f; ts = %ld]\n", dummy->data.id, dummy->data.value, dummy->data.ts); // ADDED
		else printf("\t> SBUFFER: Removing EOB.\n");
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
    if(data != NULL) dummy->data = *data;
    dummy->next = NULL;
	dummy->stop = stop_and_go;	// ADDED: SBUFFER_STOP | SBUFFER_GO
	dummy->reader_datamgr_flag = 0;
	dummy->reader_sensor_db_flag = 0;
	if(stop_and_go == SBUFFER_GO) printf("\t> SBUFFER: Inserting node [id = %d; value = %f; ts = %ld]\n", dummy->data.id, dummy->data.value, dummy->data.ts);	// ADDED
	else printf("\t> SBUFFER: Inserting node [EOB]\n");	// ADDED
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

// ADDED: function to more easily read the buffer from different threads
int sbuffer_read(sbuffer_t *buffer, sensor_data_t* data, int reader_thread){
	DEBUG_PRINT("Sbuffer: Reader_thread = %d\n", reader_thread);
	pthread_mutex_lock(&lock_buffer);
	// A bit of the same logic as in remove but checking twice is better than not checking at all
	// Here thread safety is implemented, not in the remove
	// No buffer
	if (buffer == NULL){
		pthread_mutex_unlock(&lock_buffer);
		return SBUFFER_FAILURE;
	}
	// Empty buffer
    if (buffer->head == NULL) {
		printf("\t> SBUFFER: Read on hold, waiting for data...\n");
		pthread_cond_wait(&data_inserted, &lock_buffer);	// waiting for insert to signal there is data in the buffer then read again
		pthread_mutex_unlock(&lock_buffer);
		return SBUFFER_NO_DATA;
	}
	// While loop to go through buffer to find first node this thread hasn't read
	sbuffer_node_t* dummy = buffer->head;
	while((reader_thread == READER_DATAMGR && dummy->reader_datamgr_flag) || (reader_thread == READER_SENSOR_DB && dummy->reader_sensor_db_flag)){
		dummy = dummy->next;
		if(dummy == NULL){
			printf("\t> SBUFFER: Read on hold, waiting for data...\n");
			pthread_cond_wait(&data_inserted, &lock_buffer);
			pthread_mutex_unlock(&lock_buffer);
			return SBUFFER_NO_DATA;
		}
	}
	// EOB data
	if(dummy->stop == SBUFFER_STOP){
		printf("< SBUFFER: Read received EOB, buffer will exit.\n");
		if(reader_thread == READER_DATAMGR) {
			DEBUG_PRINT("Sbuffer: Datamgr has read the EOB.\n");
			dummy->reader_datamgr_flag = 1;
		}
		if(reader_thread == READER_SENSOR_DB) {
			DEBUG_PRINT("Sbuffer: Sensor_db has read the EOB.\n");
			dummy->reader_sensor_db_flag = 1;
		}
		int sbuffer_remove_status = sbuffer_remove(buffer);
		if(sbuffer_remove_status == SBUFFER_FAILURE) printf("Sbuffer:  ERROR: tried to remove from NULL buffer.\n");
		if(sbuffer_remove_status == SBUFFER_NO_DATA) printf("Sbuffer: ERROR: tried to remove from empty buffer.\n");
		pthread_mutex_unlock(&lock_buffer);
		return SBUFFER_FINISHED;
	}
	// Actual read
	printf("\t> SBUFFER: Received data, continuing read. (If data is not printed next something went wrong when assigning the data read from sbuffer.)\n");
	if(data != NULL) *data = dummy->data;
	printf("\t> SBUFFER: Read data [thread = %d; id = %d; value = %f; ts = %ld]\n", reader_thread, data->id, data->value, data->ts);
	// Set flags to indicate current thread has read this node
	if(reader_thread == READER_DATAMGR) dummy->reader_datamgr_flag = 1;
	if(reader_thread == READER_SENSOR_DB) dummy->reader_sensor_db_flag = 1;
	// Remove node when read (remove checks if all threads have read this node)
	sbuffer_remove(buffer);
	pthread_mutex_unlock(&lock_buffer);
	return SBUFFER_SUCCESS;
}
// ADDED: this is to check if the buffer is empty
int sbuffer_size(sbuffer_t *buffer){
	if(buffer == NULL) return -1;
	int size = 0;
	if(buffer->head != NULL){
		sbuffer_node_t* dummy = buffer->head;
		size++;
		while(dummy->next){
			size++;
		}
	}
	return size;
}