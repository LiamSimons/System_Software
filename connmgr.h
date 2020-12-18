#ifndef __CONNMGR_H__
#define __CONNMGR_H__

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <inttypes.h>

#include "lib/tcpsock.h"
#include "lib/dplist.h"
#include "config.h"

#ifndef TIMEOUT
	#error TIMEOUT must be specified [in seconds]
#endif


// from poll man page

typedef struct pollfd polldescr;


typedef struct{
	polldescr file_descriptors;
	time_t last_ts;
	sensor_id_t sensor_id;
	tcpsock_t *socket_id;
}pollinfo;

typedef struct{
	pollinfo *array;
	int conn_amount;
	int timeout = TIMEOUT;
}poll_array;

// This connection manager should be using your dplist to store all the info on the active sensor nodes.

/*This method holds the core functionality of your connmgr. 
It starts listening on the given port and when a sensor node connects it writes the data to a sensor_data_recv file.
This file must have the same format as the sensor_data file in assignment 6 and 7.
*/
void connmgr_listen(int port_number);


/*This method should be called to clean up the connmgr, and to free all used memory.
After this no new connections will be accepted
*/
void connmgr_free();


/*
Labtools checklist:

	-	Checking if connmgr.c, connmgr.h and config.h are available in the ZIP file (in the root file, no sub-folder)
		Any main.c will be ignored, for this a default one will be used. 
		Also your dplist.c, dplist.h, tcpsock.c and tcpsock.h need to be available in the ZIP file,
		these should be located in a seperate folder called lib inside the ZIP file and will be compiled as shared libraries.
		This file structure is important for the test to run.

	-	Running cppcheck on the source code (parameters: --enable=all --suppress=missingIncludeSystem)

	-	Compiling your connmgr functions and checking if it produces no errors or no warnings 
		(parameters= -Wall -std=c11 -Werror -DTIMEOUT=5)

	-	Running the connection manager and testing if it only produces one process with a single thread

	- Checking the content of the sensor_data_recv file. The data sent by the sensor nodes should be available in sensor_data_recv.

*/

#endif