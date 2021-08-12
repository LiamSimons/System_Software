/**
*	\author Liam Simons
*/

#ifndef _MAIN_H_
#define _MAIN_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <wait.h>
#include <assert.h>

#include <sys/types.h>	// fifo
#include <sys/stat.h>	// fifo

#include "datamgr.h"	// lab5
#include "sensor_db.h"	// lab6
#include "connmgr.h"	// lab7
#include "sbuffer.h"	// lab8

#ifndef MAX_FIFO_CHARACTERS
#define MAX_FIFO_CHARACTERS 200
#endif

typedef struct connmgr_thread_arg connmgr_thread_arg_t;

int create_fifo();
int write_fifo();		// callback function ?
int remove_fifo();

void run_child();		// log process - Req 1.
void* connmgr();		// Req 2.
void* datamgr();		// Req 2.
void* sensor_db();		// Req 2.

int main(int argc, char *argv[]);

#endif