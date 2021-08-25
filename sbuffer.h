/**
 * \author Liam Simons
 */

#ifndef _SBUFFER_H_
#define _SBUFFER_H_

#include "config.h"
#include <pthread.h>

#define SBUFFER_FAILURE		-1
#define SBUFFER_SUCCESS		0
#define SBUFFER_NO_DATA		1
#define SBUFFER_FINISHED	2	// ADDED: to signal readers the buffer is done

#define SBUFFER_STOP		3	// ADDED: writer inserts this into buffer to signal it to stop
#define SBUFFER_GO			4	// ADDED: writer inserts this when inserting normal data into buffer

#define READER_DATAMGR		5	// ADDED: read parameter to distinguish which thread is reading from the buffer
#define READER_SENSOR_DB	6	// ADDED: read parameter to distinguish which thread is reading from the buffer

typedef struct sbuffer sbuffer_t;

/**
 * Allocates and initializes a new shared buffer
 * \param buffer a double pointer to the buffer that needs to be initialized
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_init(sbuffer_t **buffer);

/**
 * All allocated resources are freed and cleaned up
 * \param buffer a double pointer to the buffer that needs to be freed
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_free(sbuffer_t **buffer);

/**
 * Removes the first sensor data in 'buffer' (at the 'head') and returns this sensor data as '*data'
 * If 'buffer' is empty, the function doesn't block until new sensor data becomes available but returns SBUFFER_NO_DATA
 * \param buffer a pointer to the buffer that is used
 * \param data a pointer to pre-allocated sensor_data_t space, the data will be copied into this structure. No new memory is allocated for 'data' in this function.
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_remove(sbuffer_t *buffer/*, sensor_data_t *data*/);

/**
 * Inserts the sensor data in 'data' at the end of 'buffer' (at the 'tail')
 * \param buffer a pointer to the buffer that is used
 * \param data a pointer to sensor_data_t data, that will be copied into the buffer
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occured
*/
int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data, int stop_and_go);

// ADDED: function to more easily read the buffer from different threads
//		  some handling and differentiation between threads 
int sbuffer_read(sbuffer_t *buffer, sensor_data_t* data, int reader_thread);

// ADDED: this is to check if the buffer is empty
int sbuffer_size(sbuffer_t *buffer);

#endif  //_SBUFFER_H_
