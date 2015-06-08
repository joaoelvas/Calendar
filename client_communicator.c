// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "mysocks.h"
#include "client.h"




/*
 * Check if the request returns a list
 */ 
int is_list(Request* request) {
	return operation(request) == LIST_ALL || 
		operation(request) == LIST || 
		operation(request) == LIST_ONGOING_EVENTS;
}	

/*
 * Handle a request issued from the command line.
 * It sends the request to the server and waits for the reply
 *    request - the request information
 */
int communicate_event_request(Request* request) {

	int res = myWriteSocket(request->socket, (char *) &request->event_op, sizeof(EventOp));
	if (res == -1)
		return -1;
	
	if (operation(request) == ADD) {	
		myWriteSocket(request->socket, (char *) &request->event, sizeof(Event));
		if (res == -1)
			return -1;
	}
	
	
	int reply;
	res = myReadSocket(request->socket, (char *) &reply, sizeof(int));
	
	if (res == -1)
		return -1;

	if (reply == -1) 
			printf ("Error: Could not perform operation.\n");
	
	else if (reply > 0) { 			
		if (is_list(request)) {
			EventList* eventList = new_event_list(reply);	
			res = myReadSocket(request->socket, (char *) eventList->events, 
							eventList->nEvents * sizeof(Event));
			if (res == -1) 	
				return -1;

			if (operation(request) == LIST_ONGOING_EVENTS) {
				// NOT IMPLEMENTED - START
				printf("\n******************\n*  NOTIFICATION  *\n******************\n");
				print_event_list(eventList);

				// NOT IMPLEMENTED - END		
				return 0;
			}
			else 
				print_event_list(eventList);	
	
			destroy_event_list(eventList);	
		}
		else {
			switch (operation(request)) {
		
				case ADD:
					printf ("Added event number %d\n", reply);
					break;
					
				case REMOVE:
					
					printf("Removed event number %d\n", reply);
					
					break;
						
				default:	
					printf ("Internal Error - client handling operation %d\n", (-reply));
			}
		}
	}
			
	return 0;
}
	
