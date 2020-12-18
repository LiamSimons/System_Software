/*
from poll man page:
	
	#include <poll.h>

	int poll(struct pollfd *fds, nfds_t nfds, int timeout);

	#define _GNU_SOURCE		// See feature_test_macros(7) 
	#include <signal.h>
	#include <poll.h>

	int ppoll(struct pollfd *fds, nfds_t nfds,
			const struct timespec *tmo_p, const sigset_t *sigmask);
*/

#define _GNU_SOURCE

#include "connmgr.h"

// List to store the sockets	--------------------------------------------------------------------------------

dplist_t *connections;


// Callback functions			--------------------------------------------------------------------------------

void* copy_element(void *element){
	pollinfo *copy = malloc(sizeof(pollinfo));
	copy->file_descriptors = ((pollinfo*)element)->file_descriptors;
	copy->last_ts = ((pollinfo*)element)->last_ts;
	copy->sensor_id = ((pollinfo*)element)->sensor_id;
	copy->socket_id = ((pollinfo*)element)->socket_id;
	return (void*)copy;
}

void free_element(void **element){
	free(*element);
	return;
}

int compare_element(void *x, void *y){
	return 0;
}


// Implementation				--------------------------------------------------------------------------------

void connmgr_listen(int port_number){
	// start with making a list to store the connections
	connections = dpl_create(copy_element, free_element, compare_element);

	// variables - |termination boolean| - |pollinfo poll_server| - |polldescr poll_server_fd| 
	// 			 - |tcpsock_t *server| 	 - |tcpsock_t *sensor|	  -> one for each side
	bool terminated = false;
	pollinfo *poll_server = (pollinfo*)malloc(sizeof(pollinfo));
	polldescr *poll_server_fd = (polldescr*)malloc(sizeof(polldescr));
	tcpsock_t *server, *sensor;

	printf("SERVER STARTUP\n");
	// Making socket connection for the server and getting socket descriptor
	if(tcp_passive_open(&server, port_number) != TCP_NO_ERROR){
		printf("ERROR: can't connect to the SERVER.\n");
	}
	if(tcp_get_sd(server, &(poll_server_fd->fd)) != TCP_NO_ERROR){
		printf("ERROR: socket is not bounded to server.\n");
	}

	// Initialize the server at index 0
	// put file descriptor in the struct
	poll_server->file_descriptors = *poll_server_fd;
	// man page: POLLIN - there is data to read
	poll_server->file_descriptors.events = POLLIN;
	poll_server->socket_id = server;
	poll_server->last_ts = time(NULL);
	// insert into dpl_list connections
	connections = dpl_insert_at_index(connections, (void*)(poll_server), 0, true);

	printf("SERVER connected\n");

	// POLL LOOP
	do{
		// loop over all sockets in connections
		int nr_connections = dpl_size(connections);
		for(int i=0; i<nr_connections; i++){

			// poll for new connection or new data
			pollinfo *new_poll = ((pollinfo*)dpl_get_element_at_index(connections, i));
			if(poll(&(new_poll->file_descriptors), 1, 0) > 0){
				// check if data available
				if(new_poll->file_descriptors.revents == POLLIN){
					sensor_data_t data;
					int bytes, result;
					// incoming connection on server
					if(i == 0){
						if (tcp_wait_for_connection(new_poll->socket_id, &sensor) != TCP_NO_ERROR) exit(EXIT_FAILURE);
						pollinfo *new_sensor = (pollinfo*)malloc(sizeof(pollinfo));
						new_sensor->socket_id = sensor;
						if(tcp_get_sd(sensor, &(poll_server_fd->fd)) != TCP_NO_ERROR) printf("Sensor not connected.\n");
						new_sensor->file_descriptors = *poll_server_fd;
						new_sensor->file_descriptors.events = POLLIN | POLLRDHUP;
						new_sensor->last_ts = time(NULL);
						connections = dpl_insert_at_index(connections, (void*)(new_sensor), dpl_size(connections), true);
						free(new_sensor);
						printf("New sensor is connected.\n");
						printf("Total amount of sensors connected = %d\n", dpl_size(connections)-1);
						// if(tcp_receive(server, (void*) &data.id, &bytes) == TCP_CONNECTION_CLOSED){
						// 	printf("Server can't connect to sensor %d\n", new_poll->sensor_id);
						// }
					}
					// incoming data from sensor
					else{
						// from test server:


						bytes = sizeof(data.id);
			            result = tcp_receive(sensor, (void *) &data.id, &bytes);
			            // read temperature
			            bytes = sizeof(data.value);
			            result = tcp_receive(sensor, (void *) &data.value, &bytes);
			            // read timestamp
			            bytes = sizeof(data.ts);
			            result = tcp_receive(sensor, (void *) &data.ts, &bytes);

			            new_poll->sensor_id = data.id;
			            new_poll->last_ts = time(NULL);

			            if ((result == TCP_NO_ERROR) && bytes) {
			                printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
			                       (long int) data.ts);
			                FILE *fp = fopen("sensor_data_recv.txt", "a");
			                fprintf(fp, "%d %f %ld\n", data.id, data.value, data.ts);
			                fclose(fp);
			            }
			            // else if(result == TCP_CONNECTION_CLOSED){
			            // 	printf("Connection lost of sensor %d\n", new_poll->sensor_id);
			            // }
					}
				}
				// timeout of a sensor
				if((new_poll->last_ts + TIMEOUT) < time(NULL) && i != 0){
					printf("Connection lost due to timeout of sensor %d\n", new_poll->sensor_id);
					tcp_close(&(new_poll->socket_id));
					connections = dpl_remove_at_index(connections, i, true);
					poll_server->last_ts = time(NULL);
				}
				// printf("Size connections = %i\n", dpl_size(connections));
				// no sensors connected server timeout
				
			}
		}
		if(dpl_size(connections) == 1 && (poll_server->last_ts + TIMEOUT) < time(NULL)){
					printf("No more sensors connected - TIMEOUT exceeded.\nServer shutdown.\n");
					tcp_close(&(poll_server->socket_id));
					connmgr_free();
					terminated = true;
				}
	}while(!terminated);

	free(poll_server_fd);
	free(poll_server);
	return;
}

void connmgr_free(){
	dpl_free(&connections, true);
}