/**
 * \author Brecht De Vuyst
 */

#ifndef _CONNMGR_H_
#define _CONNMGR_H_

#ifndef TIMEOUT
#error TIMEOUT not set
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <inttypes.h>
#include "poll.h"
#include "lib/tcpsock.h"
#include "config.h"
//#include "sbuffer.h"
#include "lib/dplist.h"

void connmgr_listen(int port_number/*, sbuffer_t *sbuffer, FILE *fifo*/);

void connmgr_free(dplist_t *connections);

#endif