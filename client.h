// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL

#ifndef _FSO_CLIENT_H_
#define _FSO_CLIENT_H_
#include <pthread.h>
#include "event.h"

#define DEFAULT_PORT 7000

//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // NOT IMPLEMENTED

typedef struct {
	EventOp event_op;
	Event event;
	int socket;
} Request;


#define operation(X) 	((X)->event_op.type)

/** 
 * The command line interpreter
 */
void console();	

void* notification(void* x);		

/*
 * Handle a request issued from the command line.
 * It sends the request to the server and waits for the reply
 *    request - the request information
 */
int communicate_event_request(Request* request);


#endif // _FSO_CLIENT_H_
