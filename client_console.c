// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "client.h"
#include "mysocks.h"

#define LINESIZE 1024

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef enum {
	EXIT = -2,
	ERROR, 
	IGNORE,
	OK		
} ConsoleResult; // Type of user action 

static char* error_msg; // Error message

/*
 *  Console usage
 */
static void usage() {
	printf(
		"   add description day month year hour duration \n" \
		"           hour format: HHMM\n" \
		"           duration is given in minutes\n" \
		"           example: add test 2 10 2014 1805 30\n" \
		"   remove event_id\n" \
		"   list event_id\n" \
		"   listall\n"
		"   exit (or quit)\n"
	);
}

/**
 * Fill the event field of a request with the user supplied information
 *   request - the target request
 */ 
static ConsoleResult fill_event(Request* request) {
	char *token = strtok(NULL, " \t\n"); 
	if (token == NULL) {
		error_msg = "Wrong nunber of arguments.";
		return ERROR;
	}
	strcpy(description(&request->event), token);

	int ntokens = 0;
	while (ntokens < 5) {
		char *token = strtok(NULL, " \t\n");
		if (token == NULL) {
			error_msg = "Wrong nunber of arguments.";
			return -1;
		}
		request->event.data[ntokens] = atoi(token);
		ntokens++;
	}
	
	return OK;
}

/**
 * Fill the id field of a request with the user supplied information
 *   request - the target request
 */ 
static ConsoleResult fill_id(Request* request) {
	char *token = strtok(NULL, " \t\n");
	if (token == NULL) {
		error_msg = "Wrong nunber of arguments.";
		return ERROR;
	}
	id(&request->event_op) = atoi(token);
	return OK;
}

/**
 * Parse a given command line
 *   command - the command line to parse
 *   request - the request to fill
 */ 
static ConsoleResult read_request(char* command, Request* request) {
	if ( command == NULL || request == NULL) {
		error_msg = "Internal error.";
		return ERROR;
	}

	char *token = strtok(command, " \t\n");
	if (token == NULL)
		return IGNORE;
			
	if (strcmp(token, "help") == 0) {
		usage();
		return IGNORE;
	}
	
	if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0) 
		return EXIT;
	
	// Make event
		
	if (strcmp(token, "add") == 0) {
		operation(request) = ADD;	
		int res = fill_event(request);
		
		// The user supplied values are not being validated
		// If you want to do so, implement the validation functions
		// of file event.c and call them here
		
		return res;
	} 
	else if (strcmp(token, "listall") == 0) {
		operation(request) = LIST_ALL;
		return OK;
	}
	else if (strcmp(token, "list") == 0) {
		//error_msg = "Not implemented.";
		operation(request) = LIST;
		int res = fill_id(request);
		return res;
	}
	else if (strcmp(token, "remove") == 0) {
		//error_msg = "Not implemented.";
		operation(request) = REMOVE; //request->event_op.type
		int res = fill_id(request);
		return res;
	}
	else {
		error_msg = "Unknown command.";
		return ERROR;
	}
}

	// NOT IMPLEMENTED - START

void* notification(void* x) {
	Request* request = (Request*) x;

	while(1){
		pthread_mutex_lock(&lock);
		operation(request) = LIST_ONGOING_EVENTS;
		communicate_event_request(request);
		sleep(60);
		pthread_mutex_unlock(&lock);
	}
}



	// NOT IMPLEMENTED - END


/**
 * The command line interpreter
 */ 
void console(Request *request) { 
	char line[LINESIZE];
				
	printf("> "); 
	fflush(stdout); 
	
	 	
	while (fgets(line, LINESIZE, stdin) != NULL) {
		ConsoleResult res = read_request(line, request); 
		switch (res) {
			case ERROR: 
				printf("Error: %s\n", error_msg);
				break;
			
			case EXIT: 
				return;
				
			case OK:
			//pthread_mutex_lock(&lock);	// NOT IMPLEMENTED 
				communicate_event_request(request);
			//pthread_mutex_unlock(&lock); // NOT IMPLEMENTED
			
			default: // IGNORE
				break;	
		}
		
		printf("> ");
		fflush(stdout);	
	}
}	
