#define _GNU_SOURCE
#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>
#include "connmgr.h"

#ifdef DEBUG
# define DEBUG_PRINT(...) 				\
			do{							\
				printf(__VA_ARGS__);	\
			}while(0)				
#else
# define DEBUG_PRINT(...) do {} while (0)
#endif

#ifndef LOADING_TIME
#define LOADING_TIME 3
#endif


// VARIABLES ----------------------------------------------------------------------//
	// DPLIST
	dplist_t* connections;
	// POLL 
	//int nfds, num_open_fds; //TO BE REVIEWED//
	typedef struct pollfd polldescr;
// --------------------------------------------------------------------------------//

// STRUCTS ------------------------------------------------------------------------//
	typedef struct{
		tcpsock_t* socket;
		polldescr poll_fd; 		// first tried without this but wouldn't work
		sensor_data_t* data;	// a bit redundant data, but was most logic solution for me
		time_t last_ts;			// is already in data but this is clearer and more accessible
	}list_element;
// --------------------------------------------------------------------------------//

// EXTRA FUNCTIONS DECLARATION-----------------------------------------------------//
static void server_print(char to_print[]);	//print template for when server is running
static void loading_dots(int seconds);		//just for fun
static int update_poll_array(polldescr** poll_array, int nr_connections); //updates poll_array
	// 0 	= success
	// -1 	= failure
// --------------------------------------------------------------------------------//

// CALLBACK FUNCTIONS -------------------------------------------------------------//
	void* element_copy(void* element){
		sensor_data_t* data_copy = (sensor_data_t*) malloc(sizeof(sensor_data_t));
		data_copy->id = ((list_element*)element)->data->id;
		data_copy->value = ((list_element*)element)->data->value;
		data_copy->ts = ((list_element*)element)->data->ts;

		list_element* copy = (list_element*)malloc(sizeof(list_element));
		copy->socket = ((list_element*)element)->socket;
		copy->poll_fd = ((list_element*)element)->poll_fd;
		copy->data = data_copy;
		copy->last_ts = ((list_element*)element)->last_ts;
		
		return copy;
	}
	void element_free(void** element){
		list_element* dummy = (list_element*)(*element);
		sensor_data_t* dummy_data = dummy->data;
		free(dummy_data);
		free(*element);
	}
	int element_compare(void* x, void* y){
		return ((((list_element*)x)->data->id < ((list_element*)y)->data->id) ? -1 :
		 (((list_element*)x)->data->id == ((list_element*)y)->data->id) ? 0 : 1);
	}
// --------------------------------------------------------------------------------//



// CONNMGR IMPLEMENTATION ---------------------------------------------------------//
void connmgr_listen(int port_number){
	DEBUG_PRINT("#connmgr_listen() - called#\n");

	// Dplist and pollarray
	connections = dpl_create(element_copy, element_free, element_compare);
	polldescr* poll_array = (polldescr*)malloc(sizeof(polldescr));

	// Variables
	tcpsock_t *server, *sensor;
	int bytes, result;

	// Server startup------------------------------------------------------------//
	printf("< Server starting up...\n");
		// Server socket
		if(tcp_passive_open(&server, port_number) != TCP_NO_ERROR) {
			printf("Failure: server socket can't be opened.");
			exit(EXIT_FAILURE);
		} 
		DEBUG_PRINT("\tServer socket successfully opened\n");

		// Server list element
		sensor_data_t* server_data = (sensor_data_t*)malloc(sizeof(sensor_data_t));
		server_data->id = 0;
		server_data->value = 0;
		server_data->ts = time(NULL);
		DEBUG_PRINT("\tServer data made\n");

		struct pollfd server_polldescr;
		if(tcp_get_sd(server, &(server_polldescr.fd)) != TCP_NO_ERROR){
			printf("Failure: can't retrieve server sd.");
			exit(EXIT_FAILURE);
		}
		DEBUG_PRINT("\tServer sd retrieved\n");

		list_element* server_element = (list_element*)malloc(sizeof(list_element));
		server_element->socket = server;
		server_element->poll_fd = server_polldescr;
		server_element->data = server_data;
		server_element->last_ts = time(NULL);
		poll_array->events = POLLIN;

		DEBUG_PRINT("\tServer element made\n");
		connections = dpl_insert_at_index(connections, (void*)(server_element), 0, true);

	printf("< Server ready:\t[timeout after %d seconds]\n", TIMEOUT);
	// -------------------------------------------------------------------------//

	// MAIN LOOP ---------------------------------------------------------------// 
		do{
			// POLL ------------------------------------------------------------//
			int nr_connections = dpl_size(connections);
			DEBUG_PRINT("\tNr connections = %d\n", nr_connections);
			// Update the poll_array
			if(update_poll_array(&poll_array, nr_connections) != 0){
				server_print("Failure: error updating the poll_array");
				exit(EXIT_FAILURE);
			}
			DEBUG_PRINT("\tUpdated poll_array.\n");
			DEBUG_PRINT("\tStarting poll:\n");
			int poll_value = poll(poll_array, nr_connections, TIMEOUT*1000);
			DEBUG_PRINT("\tPoll finished.\n");

			if(poll_value == -1) exit(EXIT_FAILURE);

			// POLL returns 0 => timeout reached -------------------------------//
			if(poll_value == 0){
				// Only server remains
				if(nr_connections == 1){
					printf("Server timeout: ");
					printf("Freeing memory ");
					loading_dots(LOADING_TIME);
					tcp_close(&server);
					free(server_data);
					free(server_element);
					free(poll_array);
					connmgr_free();
					printf("Memory clear\n");
					break;
				}
				// There are still sensors but they haven't sent data before timeout
				else{
					// Disconnect all sensors and poll again?
					// or just continue?
				}
			}
			// POLL returns 1+ -------------------------------------------------//
			//else{
				// Check if server node received a new connection --------------//
				
				// Otherwise check which sensor sent data ----------------------//
				
					
					//server_print("Server received data:");
					for(int sensor_index = 1; sensor_index < nr_connections; sensor_index++){
						list_element* sensor_element = dpl_get_element_at_index(connections, sensor_index);
						tcpsock_t* socket = sensor_element->socket;

						// Check for sensors that are over timeout
						if((sensor_element->last_ts + TIMEOUT) < time(NULL)){
							server_print("A sensor reached its timeout limit");
							poll_array[sensor_index].revents = 0;
							tcp_close(&(socket));
							connections = dpl_remove_at_index(connections, sensor_index, true);
							// Update the poll_array
							if(update_poll_array(&poll_array, dpl_size(connections)) != 0){
								server_print("Failure: error updating the poll_array");
								exit(EXIT_FAILURE);
							}
						}

						// Check if a sensor is polling to send data
						if(poll_array[sensor_index].revents == POLLIN){
							DEBUG_PRINT("SENSOR IS POLLIN");
							sensor_data_t* dummy_data = sensor_element->data;
								// read in
								bytes = sizeof(dummy_data->id);
								result = tcp_receive(sensor, (void *) &(dummy_data->id), &bytes);
								// read temperature
								bytes = sizeof(dummy_data->value);
								result = tcp_receive(sensor, (void *) &(dummy_data->value), &bytes);
								// read timestamp
								bytes = sizeof(dummy_data->ts);
								result = tcp_receive(sensor, (void *) &(dummy_data->ts), &bytes);
								sensor_element->last_ts = time(NULL);
							if((result == TCP_NO_ERROR) && bytes){
								printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", dummy_data->id, dummy_data->value,
			                       (long int) dummy_data->ts);
								FILE *fp = fopen("sensor_data_recv.txt", "a");
								fprintf(fp, "%d %f %ld\n", dummy_data->id, dummy_data->value, dummy_data->ts);
								fclose(fp);
							}
							if(result == TCP_CONNECTION_CLOSED){
								server_print("Sensor closed connection.");
								connections = dpl_remove_at_index(connections, sensor_index, true);
							}
							//poll_array[sensor_index].revents = 0;
						}
					}
				if(poll_array[0].revents == POLLIN){
					// Sensor socket
						server_print("Server received new sensor request");
						if(tcp_wait_for_connection(server_element->socket, &sensor) != TCP_NO_ERROR){
							server_print("Failure: new sensor failed to connect.");
							exit(EXIT_FAILURE);
						}
						server_print("New sensor connected");
						struct pollfd sensor_polldescr;
						if(tcp_get_sd(server, &(sensor_polldescr.fd)) != TCP_NO_ERROR){
							printf("Failure: can't retrieve server sd.");
							exit(EXIT_FAILURE);
						} 

					// Sensor list element
					list_element *dummy_element = (list_element*)malloc(sizeof(list_element));
					sensor_data_t *data = (sensor_data_t*)malloc(sizeof(sensor_data_t));
						data->id = 0;
						data->value = 0;
						data->ts = time(NULL);
						/*// read first data
						bytes = sizeof(data->id);
			            result = tcp_receive(sensor, (void *) &(data->id), &bytes);
			            // read temperature
			            bytes = sizeof(data->value);
			            result = tcp_receive(sensor, (void *) &(data->value), &bytes);
			            // read timestamp
			            bytes = sizeof(data->ts);
			            result = tcp_receive(sensor, (void *) &(data->ts), &bytes);*/

						dummy_element->socket = sensor;
						dummy_element->poll_fd = sensor_polldescr;
						dummy_element->data = data;
						dummy_element->last_ts = time(NULL);

					connections = dpl_insert_at_index(connections, (void*)dummy_element, nr_connections, true);
						// Once inserted allocated memory not needed anymore, copy_element = true
						free(data);
						free(dummy_element);
					printf("\t> Total amount of sensors connected = %d\n", dpl_size(connections) - 1);



					/*if ((result == TCP_NO_ERROR) && bytes) {
						printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data->id, data->value,
								(long int) data->ts);
						FILE *fp = fopen("sensor_data_recv.txt", "a");
						fprintf(fp, "%d %f %ld\n", data->id, data->value, data->ts);
						fclose(fp);
					}*/
					//poll_array[0].revents = 0;
				}
				
			//}
			//------------------------------------------------------------------//
		}while(1);
	// -------------------------------------------------------------------------//

	printf("< The server shut down;\n");
	DEBUG_PRINT("#connmgr_listen() - finished#\n");
	exit(EXIT_SUCCESS);
}

void connmgr_free(){
	DEBUG_PRINT("\n#connmgr_free() - called#\n");
	dpl_free(&connections, true);
	DEBUG_PRINT("#connmgr_free() - finished#\n");
}
// --------------------------------------------------------------------------------//

// EXTRA FUNCTIONS  IMPLEMENTATION ------------------------------------------------//
static void server_print(char to_print[]){
	printf("\t> %s\n", to_print);
}
static void loading_dots(int seconds){
	for(int i = 0; i < seconds; i++){
		printf(".");
		fflush(stdout);
		sleep(1);
	}
	printf(" ");
}
static int update_poll_array(polldescr** poll_array, int nr_connections){
	// Reallocate poll struct array to new size, 
	// add fd for each connection in list and set to polling
	*poll_array = (polldescr*)realloc(*poll_array, (sizeof(polldescr)*nr_connections));
	//DEBUG_PRINT("SIZE GIVEN TO UPDATE = %d\n", dpl_size(connections));
	if(*poll_array == NULL) return -1;
	for(int con = 0; con < nr_connections; con++){
		list_element* temp_element = (list_element*)dpl_get_element_at_index(connections, con);
		//temp_element->last_ts = time(NULL);
		if(tcp_get_sd(temp_element->socket, &(((*poll_array)[con]).fd)) != TCP_NO_ERROR) return -1;
		//temp_element->poll_fd.events = POLLIN;
		(*poll_array)[con].events = POLLIN;
	}
	return 0;
}
// --------------------------------------------------------------------------------//