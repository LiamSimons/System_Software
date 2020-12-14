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
	return;
}

void connmgr_free(){
	dpl_free(&connections, true);
}