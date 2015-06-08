// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "client.h"
#include "mysocks.h"


static Request request;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;// NOT IMPLEMENTED

/**
 *  Client program usage
 */
static void print_client_usage(char* exec_name) {
	printf ("Usage: %s username (max %d characters) [-h host] [-p port]\n" \
      		"  -h name of the host running the server. The default is localhost.\n" \
      		"  -p port where the server is listening for requests. The default is %d. \n",
      		exec_name, MAX_USER, DEFAULT_PORT); 
}

/**
 * Set the username for the given request
 */
static void set_request_user(Request *request, char* username) {
	strcpy(user(&request->event_op), username);
}


/**
 * Main function
 */		
int main(int argc,  char* argv[]) {

	// Validate command line
	if (argc < 2 || !validate_user(argv[1])) {
		print_client_usage(argv[0]);
		return 1;
	} 
	
	// Set default values
	char* hostname = "localhost";
	int port = DEFAULT_PORT; 
 
 	// Process command line options
	char c;
	while ((c = getopt (argc-1, argv+1, "h:p:")) != -1) {
  	  	switch (c) {
    		case 'h':
      			hostname = optarg;
       			break;
       			
      		case 'p':
        		port = atoi(optarg);
        		break;
        		
      		default:
        		print_client_usage(argv[0]);
        		return 1;
    	}
    }

	// Connect to server
	request.socket = myConnectSocket(hostname, port);
	if (request.socket == -1) {
		perror("Error");
		return 1;
	}

	// Set user information
	set_request_user(&request, argv[1]);
	


	Request *ptr = &request;

	// NOT IMPLEMENTED - START
	pthread_t notif;
	pthread_create(&notif, NULL, notification, (void*)ptr);


	// NOT IMPLEMENTED - END

	// Run console
	pthread_mutex_lock(&lock1);
	console(&request);
	pthread_mutex_unlock(&lock1);

	
	// Close connection and exit
	myCloseSocket(request.socket);
	return 0;
}
