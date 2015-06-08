// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include "calendar.h"
#include "mysocks.h"

#define DEFAULT_PORT 7000

static int server_socket;

// NOT IMPLEMENTED - START
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// NOT IMPLEMENTED - END

/**
 * The function to handle incoming client requests
 *    socket - the client socket descriptor
 *
 * The function returns to client:
 *    -1, if an error occurred in the processing of the request
 *    the id of the event, if ADD or REMOVE were successfully executed 
 *    the size of the list of events + the list itself, if a LIST* operation was successfully executed 
 */
void* handleClient(void* s) {
	int socket = *(int*) s;
	int nBytes;
	EventOp event_op;
	
	printf("Handling new client at socket %d.\n", socket);
	
	while ((nBytes = myReadSocket(socket, (char *) &event_op, sizeof(EventOp))) > 0) {
		
		int reply;
		switch (event_op.type) {
		
		case ADD: {
			pthread_mutex_lock(&lock); // NOT IMPLEMENTED

			printf("Handling ADD request from %s \n", event_op.user);
		
			Event event;
			nBytes = myReadSocket(socket, (char *) &event, sizeof(Event));
			strcpy(user(&event), event_op.user);
			
			if (nBytes == -1) 
				reply = -1;
			else {
				if (calendar_add(&event) == -1)
					reply = -1;
				else
					reply = event.id;
			}	
			myWriteSocket(socket, (char *) &reply, sizeof(int));
			pthread_mutex_unlock(&lock); // NOT IMPLEMENTED
			break;
		}
			 
		case REMOVE: {
			pthread_mutex_lock(&lock); //NOT IMPLEMENTED

			printf("Handling REMOVE request from %s\n", event_op.user);
	
			// NOT IMPLEMENTED - START

			if (calendar_remove(event_op.id) == -1)
					reply = -1;
				else
					reply = event_op.id;

			myWriteSocket(socket, (char *) &reply, sizeof(int));

			pthread_mutex_unlock(&lock);

			// NOT IMPLEMENTED - END

			break;
		}	
			
		case LIST: {
			pthread_mutex_lock(&lock); // NOT IMPLEMENTED

			printf("Handling LIST request from %s\n", event_op.user);
		
			// NOT IMPLEMENTED - START

			EventList* eventList = calendar_list_event(event_op.id);
			if (eventList == NULL) {
				int reply = -1; 
				myWriteSocket(socket, (char *) &reply, sizeof(int));
			}
			else
			{
				myWriteSocket(socket, (char *) &eventList->nEvents, sizeof(int));
				if (eventList->nEvents)
					myWriteSocket(socket, (char *) eventList->events, sizeof(Event) * eventList->nEvents);
				destroy_event_list(eventList);
			}

			pthread_mutex_unlock(&lock); // NOT IMPLEMENTED
			// NOT IMPLEMENTED - END

			break;	
		}
		
		case LIST_ALL: {
			pthread_mutex_lock(&lock); // NOT IMPLEMENTED

			printf("Handling LIST ALL request from %s \n", event_op.user);
		
			EventList* eventList = calendar_list();
			if (eventList == NULL) {
				int reply = -1; 
				myWriteSocket(socket, (char *) &reply, sizeof(int));
			}
			else {	
				myWriteSocket(socket, (char *) &eventList->nEvents, sizeof(int));
				if (eventList->nEvents)
					myWriteSocket(socket, (char *) eventList->events, sizeof(Event) * eventList->nEvents);
				destroy_event_list(eventList);
			}

			pthread_mutex_unlock(&lock); // NOT IMPLEMENTED
			break;	
		}
		
		case LIST_ONGOING_EVENTS: {
			printf("Handling LIST_ONGOING_EVENTS request from %s \n", event_op.user);	
			
			time_t t = time(NULL);
			struct tm *tm_struct = localtime(&t);
			EventList* eventList = calendar_list_ongoing(
				tm_struct->tm_mday, tm_struct->tm_mon+1, tm_struct->tm_year+1900, 
				tm_struct->tm_hour*100 + tm_struct->tm_min);
	
			if (eventList == NULL) {
				int reply = 0;  // 0 is ignored on the client side
				myWriteSocket(socket, (char *) &reply, sizeof(int));
			}
			else {		
				myWriteSocket(socket, (char *) &eventList->nEvents, sizeof(int));
				if (eventList->nEvents)
					myWriteSocket(socket, (char *) eventList->events, sizeof(Event) * eventList->nEvents);
				destroy_event_list(eventList);
			}
			break;
		}
		
		default:
			printf ("Internal error: Unknown request type %d.\n", event_op.type);	
		}	
	}

	close(socket);
	printf("Closed client connection.\n");
	
	if (nBytes == -1) 
		perror("");

	return NULL;
}

/**
 * Print server usage
 */ 
void print_server_usage(char* exec_name) {
	printf ("Usage: %s [-c] [-p port] [-n calendar]\n" \
      		"  -c to create the database, destroys existing one.\n" \
      		"  -p to set the port of the server socket. The default is %d. \n" \
      		"  -n to set the name of the calendar. The default is %s. \n",
      		exec_name, DEFAULT_PORT, DEFAULT_CALENDAR_NAME); 
}

int main(int argc, char* argv[]) {

	int port = DEFAULT_PORT; 
	char* calendarName = DEFAULT_CALENDAR_NAME;
	int create = 0;  

	// Process command line options
	char c;	
	while ((c = getopt (argc, argv, "cp:n:")) != -1)
		switch (c) {
    		case 'c':
      			create = 1;
       			break;
       		
	      	case 'p':
    	    	port = atoi(optarg);
        		break;
        
	        case 'n':
    	    	calendarName = optarg;
        		break;
        		
      		default:
        		print_server_usage(argv[0]);
				return 1;
     	}
 
 	// Create or open calendar database	
 	if (create) {
 		if (new_calendar_with_name(calendarName) == -1) {
      			printf("Error creating calendar %s: %s\n", calendarName, calendar_error_msg());
      			print_server_usage(argv[0]);
      			return 1;
      	}
    }
    else {
 		if (calendar_open_with_name(calendarName) == -1) {
      		printf("Error opening calendar %s: %s\n", calendarName, calendar_error_msg());
      		print_server_usage(argv[0]);
      		return 1;
    	}
    }
 
 
    // Creating the socket to receive incoming connections
	server_socket = myServerSocket(port);
    if (server_socket == -1) {
    	perror("");
    	return -1;
  	}
           		
    // Client connection handling loop
    while (1) {
		int client = myAcceptServerSocket(server_socket);    
		if (client == -1) 
    		perror("Error accepting client");
    	else {
    		// NOT IMPLEMENTED - START
    		pthread_t t;
    		pthread_create(&t, NULL, handleClient, (void*)(&client));
    	}
    		// NOT IMPLEMENTED - END

    		//handleClient(client); // handle the client
	}
    
    return 0;
}
