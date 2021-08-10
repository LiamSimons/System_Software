#include "connmgr.h"

#ifdef DEBUG
# define DEBUG_PRINT(...) 				\
			do{							\
				printf("DEBUG: ");		\
				printf(__VA_ARGS__);	\
			}while(0)				
#else
# define DEBUG_PRINT(...) do {} while (0)
#endif

// Variables and struct
dplist_t* connections;
typedef struct pollfd pollfd_t;	// from poll man page 

typedef struct list_element{
	tcpsock_t* socket;
	sensor_id_t id;
	time_t last_ts;
}list_element_t;

// Extra functions - declaration
static void server_print(char to_print[]);	// print template while server is running
static void loading_dots(int seconds);		// just for fun
static int update_poll_array(pollfd_t** poll_array, int nr_connections);
static int receive_data(sensor_data_t* data_pointer, tcpsock_t* sensor_socket);

// Callback functions
void* element_copy(void* element){
	list_element_t* copy = (list_element_t*)malloc(sizeof(list_element_t));
	copy->socket = ((list_element_t*)element)->socket;
	copy->id = ((list_element_t*)element)->id;
	copy->last_ts = ((list_element_t*)element)->last_ts;	
	return copy;
}
void element_free(void** element){
	if(*element == NULL) return;
	free(*element);
	*element = NULL;
}
int element_compare(void* x, void*y){
	return(	(((list_element_t*)x)->id < ((list_element_t*)y)->id) ? -1 :
			(((list_element_t*)x)->id == ((list_element_t*)y)->id) ? 0 :
			1);
}


//------------------------------------------------------------------------------------//
// Connmgr implementation
void connmgr_listen(int port_number){
	connections = dpl_create(element_copy, element_free, element_compare);
	pollfd_t* poll_array = (pollfd_t*)malloc(sizeof(pollfd_t));

	tcpsock_t* server, *sensor;
	sensor_data_t data;

	// Server
	if(tcp_passive_open(&server, port_number) != TCP_NO_ERROR) exit(EXIT_FAILURE);
	if(tcp_get_sd(server, &(poll_array->fd)) != TCP_NO_ERROR){
		printf("Failure: server sd cannot be retrieved");
	}
	poll_array->fd = POLLIN;
	list_element_t* server_element = (list_element_t*)malloc(sizeof(list_element_t));
		server_element->socket = server;
		server_element->id = 0;
		server_element->last_ts = time(NULL);
	connections = dpl_insert_at_index(connections, (void*)server_element, 0, true);
	DEBUG_PRINT("Server element added in connections list:\n");
	DEBUG_PRINT("\tdpl_size = %d\n", dpl_size(connections));
	DEBUG_PRINT("\tid = %d\n", server_element->id);
	free(server_element);
	printf("< Server started: [timeout after %d seconds]\n", TIMEOUT);

	// Main loop
	do{
		if(update_poll_array(&poll_array, dpl_size(connections)) != 0) exit(EXIT_FAILURE);
		DEBUG_PRINT("Poll array updated.\n");
		int poll_value = poll(poll_array, dpl_size(connections), TIMEOUT*1000);
		DEBUG_PRINT("Poll completed.\n");

		if(poll_value == -1) exit(EXIT_FAILURE);
		if(poll_value == 0 && dpl_size(connections) == 1){
			server_print("Server exceeded timeout limit.");
			server_print("Freeing memory");
			loading_dots(0);	// This is for fun: replace 0 with LOADING_TIME macro
			tcp_close(&server);
			free(poll_array);
			connmgr_free();
			printf("Memory cleared.\n");
			break;
		}
		if(poll_array[0].revents == POLLIN){
			if(tcp_wait_for_connection(server, &sensor) != TCP_NO_ERROR) exit(EXIT_FAILURE);
			server_print("New Sensor connection");
			list_element_t* sensor_element = (list_element_t*)malloc(sizeof(list_element_t));
				sensor_element->socket = sensor;
				sensor_element->id = 0; // temporary
				sensor_element->last_ts = time(NULL);
			connections = dpl_insert_at_index(connections, (void*)sensor_element, dpl_size(connections), true);
			free(sensor_element);
			if(update_poll_array(&poll_array, dpl_size(connections)) != 0) exit(EXIT_FAILURE);
		}
		for(int connection = 1; connection < dpl_size(connections); connection++){
			list_element_t* sensor = dpl_get_element_at_index(connections, connection);
			if((sensor->last_ts + TIMEOUT) < time(NULL)){
				printf("Sensor %d exceeded timeout limit.", sensor->id);
				tcp_close(&(sensor->socket));
				connections = dpl_remove_at_index(connections, connection, true);
			}
			if(poll_array[connection].revents == POLLIN){
				sensor->last_ts = time(NULL);
				int result = receive_data(&data, sensor->socket);
				if(sensor->id == 0) sensor->id = data.id;
				if(result == TCP_CONNECTION_CLOSED){
					server_print("Sensor closed connection.");
					tcp_close(&(sensor->socket));
					connections = dpl_remove_at_index(connections, connection, true);
				}
			}
		}
		
	}while(1);
	DEBUG_PRINT("While loop finished\n");
	printf("< Server closed.\n");
}
void connmgr_free(){
	dpl_free(&connections, true);
}
//------------------------------------------------------------------------------------//


// Extra functions implementation
static void server_print(char to_print[]){
	printf("\t> %s\n", to_print);
}
static void loading_dots(int seconds){
	printf("\t> ");
	for(int i = 0; i < seconds; i++){
		printf(".");
		fflush(stdout);
		sleep(1);
	}
	printf(" ");
}
static int update_poll_array(pollfd_t** poll_array, int nr_connections){
	*poll_array = (pollfd_t*)realloc(*poll_array, (sizeof(pollfd_t)*nr_connections));
	for(int con = 0; con < nr_connections; con++){
		list_element_t* temp_element = (list_element_t*)dpl_get_element_at_index(connections, con);
		if(tcp_get_sd(temp_element->socket, &(((*poll_array)[con]).fd)) != TCP_NO_ERROR) return -1;
		((*poll_array)[con]).events = POLLIN;
	}
	return 0;
}
static int receive_data(sensor_data_t* dummy_data, tcpsock_t* sensor){
	if(dummy_data == NULL || sensor == NULL) {
		printf("< Failure: receive data got NULL arguments.\n");
		return -1;
	}
	int bytes, result;
	// read id
	bytes = sizeof(dummy_data->id);
	result = tcp_receive(sensor, (void *) &(dummy_data->id), &bytes);
	// read temperature
	bytes = sizeof(dummy_data->value);
	result = tcp_receive(sensor, (void *) &(dummy_data->value), &bytes);
	// read timestamp
	bytes = sizeof(dummy_data->ts);
	result = tcp_receive(sensor, (void *) &(dummy_data->ts), &bytes);
	if((result == TCP_NO_ERROR) && bytes){
		printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", dummy_data->id, dummy_data->value,
			(long int) dummy_data->ts);
		FILE *fp = fopen("sensor_data_recv", "a");
		fprintf(fp, "%d %f %ld\n", dummy_data->id, dummy_data->value, dummy_data->ts);
		fclose(fp);
	}
	return result;
}
