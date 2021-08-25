#ifndef __CONNMGR_H__
#define __CONNMGR_H__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <sys/poll.h>
#include <unistd.h>

#include "lib/tcpsock.h"
#include "lib/dplist.h"
#include "config.h"
#include "sbuffer.h"
#include "main.h"

#ifndef TIMEOUT
	#error TIMEOUT must be specified [in seconds]
#endif

#ifndef LOADING_TIME
#define LOADING_TIME 3
#endif

// This connection manager should be using your dplist to store all the info on the active sensor nodes.

/*This method holds the core functionality of your connmgr. 
It starts listening on the given port and when a sensor node connects it writes the data to a sensor_data_recv file.
This file must have the same format as the sensor_data file in assignment 6 and 7.
*/
void connmgr_listen(int port_number, sbuffer_t* sbuffer, FILE* log_fifo);


/*This method should be called to clean up the connmgr, and to free all used memory.
After this no new connections will be accepted
*/
void connmgr_free();

#endif