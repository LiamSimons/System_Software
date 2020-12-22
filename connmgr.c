/*
struct pollfd{
	int fd; 		// file descriptor for an open file
	short events;	// requested events		- if 0 only events in revents returned are:
												1	POLLERR
												2	POLLHUP
												3	POLLNVAL
	short revents;	// returned events
}

poll(struct pollfd *fds, nfds_t nfds, int timeout);

	1	fds	  	=	is the array of polldescr structs
	2	nfds    =	number of polldescr in the array - keep this number by size of the dplist
	3	timeout	=	specifies number of milliseconds poll() should block waiting for file descriptor
					-> negatives value is infinite timeout
					-> 0 causes poll() to return immediately

Call will block until:
	
	1	a file descriptor becomes ready
	2	call is interrupted by a signal handler
	3	timeout expires
*/


#include "connmgr.h"

// The array to store the structs of each connections 		-------------------------------------------------------------------

typedef struct pollfd polldescr;


// The struct of one element to store in a linked list for easy handling	---------------------------------------------------

typedef struct{
	sensor_data_t *data;
	tcpsock_t *socket;
	time_t last_ts;
}list_element;

dplist_t *connections;


// Callback functions		----------------------------------------------------------------------------------------------------

void* copy_element(void *element){
	list_element *copy = malloc(sizeof(list_element));
	//copy->data = ((list_element*)element)->data;
	copy->socket = ((list_element*)element)->socket;
	return (void*)copy;
}

void free_element(void **element){
	free(*element);
}

int compare_element(void *x, void *y){
	// unused for now
	return 0;
}


// Implementation			----------------------------------------------------------------------------------------------------

void connmgr_listen(int port_number){
	polldescr *poll_array;
	connections = dpl_create(copy_element, free_element, compare_element);

	// variables
	bool quit = false;
	poll_array = (polldescr*)malloc(sizeof(polldescr));
	list_element *server_element = (list_element*)malloc(sizeof(list_element));
	tcpsock_t *server, *sensor;


	printf("Server is starting up.\n");

	// socket for server
	if(tcp_passive_open(&server, port_number) != TCP_NO_ERROR){
		printf("ERROR: Can't connect SERVER.\n");
		quit = true;
	}
	if(tcp_get_sd(server, &(poll_array->fd)) != TCP_NO_ERROR){
		printf("ERROR: Socket can't bound to server.\n");
		quit = true;
	}

	// man page: POLLIN - there is data to read
	poll_array->events = POLLIN;

	// Put server in dplist
	server_element->socket = server;
	server_element->last_ts = time(NULL);
	sensor_data_t *dummy_server_data = (sensor_data_t*)malloc(sizeof(sensor_data_t));;
	dummy_server_data->id = 0;
	dummy_server_data->value = 0;
	dummy_server_data->ts = time(NULL);
	server_element->data = dummy_server_data;
	connections = dpl_insert_at_index(connections, (void*)(server_element), 0, true);

	printf("Server connected.\n");

	// Server main loop
	do{
		int nr_connections = dpl_size(connections);

		// poll if anything is available
		int poll_value = poll(poll_array, nr_connections, TIMEOUT*1000);

		// after timeout poll returns 0 so go shut down server
		if(poll_value == 0){
			if(nr_connections == 1){
				tcp_close(&(server_element->socket));
				free(poll_array);
				connmgr_free();
				quit = true;
				printf("Server timeout.\n");
			}
			else{
				poll_array = realloc(poll_array, sizeof(polldescr));
				poll_array->fd = 0;

				list_element *dummy_server = dpl_get_element_at_index(connections, 0);
				dpl_free(&connections, true);
				connections = dpl_create(copy_element, free_element, compare_element);
				connections = dpl_insert_at_index(connections, dummy_server, 0, true);
				printf("All sensors disconnect, timeout reached.\nServer will shut down after 5 seconds if no new sensors active.\n");
			}
		}
		// MAIN PART
		else{
			list_element *dummy_element = (list_element*)malloc(sizeof(list_element));
			sensor_data_t data;
			int bytes, result;
			for(int i = 0; i < nr_connections; i++){
				
				if(poll_array[i].revents == POLLIN){
					if(i == 0){
						if(tcp_wait_for_connection(server_element->socket, &sensor) != TCP_NO_ERROR) exit(EXIT_FAILURE);

						printf("New sensor is connected.\n");

						// new list element for sensor
						list_element *new_sensor = (list_element*)malloc(sizeof(list_element));

						// read first data
						bytes = sizeof(data.id);
			            result = tcp_receive(sensor, (void *) &data.id, &bytes);
			            // read temperature
			            bytes = sizeof(data.value);
			            result = tcp_receive(sensor, (void *) &data.value, &bytes);
			            // read timestamp
			            bytes = sizeof(data.ts);
			            result = tcp_receive(sensor, (void *) &data.ts, &bytes);

			            dummy_element->last_ts = time(NULL);


						new_sensor->socket = sensor;
						new_sensor->last_ts = time(NULL);

						sensor_data_t *dummy_data = (sensor_data_t*)malloc(sizeof(sensor_data_t));
						dummy_data->id = data.id;
						dummy_data->value = data.value;
						dummy_data->ts = data.ts;
						new_sensor->data = dummy_data;
						connections = dpl_insert_at_index(connections, (void*)new_sensor, nr_connections, true);
						// realloc the array
						// int polldescr_size = sizeof(polldescr);
						// int connections_size = dpl_size(connections);
						// int new_alloc_size = polldescr_size*connections_size;
						poll_array = realloc(poll_array, (sizeof(polldescr)*(dpl_size(connections))));

						for(int j = 0; j < dpl_size(connections); j++){
							list_element *temp_element = (list_element*)dpl_get_element_at_index(connections, j);
							if(tcp_get_sd(temp_element->socket, &(poll_array[j].fd)) != TCP_NO_ERROR) printf("error\n");;
							poll_array[j].events = POLLIN;
						}

						printf("Total amount of sensors connected = %i\n", dpl_size(connections)-1);

						if ((result == TCP_NO_ERROR) && bytes) {
			                printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
			                       (long int) data.ts);
			                FILE *fp = fopen("sensor_data_recv.txt", "a");
			                fprintf(fp, "%d %f %ld\n", data.id, data.value, data.ts);
			                fclose(fp);
			            }
						
						free(new_sensor);
					}
					else{
						dummy_element = (list_element*)dpl_get_element_at_index(connections, i);
						sensor_data_t data;
			            result = tcp_receive(sensor, (void *) &data.id, &bytes);
			            // read temperature
			            bytes = sizeof(data.value);
			            result = tcp_receive(sensor, (void *) &data.value, &bytes);
			            // read timestamp
			            bytes = sizeof(data.ts);
			            result = tcp_receive(sensor, (void *) &data.ts, &bytes);

			            dummy_element->last_ts = time(NULL);

			            if ((result == TCP_NO_ERROR) && bytes) {
			                printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
			                       (long int) data.ts);
			                FILE *fp = fopen("sensor_data_recv.txt", "a");
			                fprintf(fp, "%d %f %ld\n", data.id, data.value, data.ts);
			                fclose(fp);
			            }

			            if(result == TCP_CONNECTION_CLOSED){
			            	printf("Sensor connection closed.\n");
			            	connections = dpl_remove_at_index(connections, i, true);
			            	poll_array = realloc(poll_array, (sizeof(polldescr)*dpl_size(connections)));
							for(int j = 0; j < dpl_size(connections); j++){
								list_element *temp_element = (list_element*)dpl_get_element_at_index(connections, j);
								if(tcp_get_sd(temp_element->socket, &(poll_array[j].fd)) != TCP_NO_ERROR) printf("error\n");;
								poll_array[j].events = POLLIN;
							}
			            	
			            	if(dpl_size(connections) == 1){
			            		printf("All sensors disconnected.\nServer will shut down after 5 seconds if no new sensors active.\n");
			            	}
			            }
					}
				}
			}
		}
	}while(!quit);

}

void connmgr_free(){
	// polldescr struct is cleared in connmgr_listen() after a timeout
	dpl_free(&connections, true);
}