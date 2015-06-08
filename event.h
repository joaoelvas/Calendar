// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL

#ifndef _FSO_EVENT_H_
#define _FSO_EVENT_H_


#define MAX_USER 10		// Limit defined to simplify the communication protocol
#define MAX_DESC 100	// Limit defined to simplify the communication protocol

// Type of an operator over events
typedef enum {
	ADD,
	REMOVE,
	LIST,
	LIST_ALL,
	LIST_ONGOING_EVENTS 
} EventOpType;

// An operation over an event
typedef struct {
	EventOpType type;
	unsigned int id;
	char user[MAX_USER+1];
} EventOp;

// An event
typedef struct {    
	unsigned int id;
	char user[MAX_USER+1];
	char description[MAX_DESC+1];	
	unsigned short data[5];	
} Event;

// A list of events
typedef struct {
	Event* events;
	int nEvents;
} EventList;


// Set of macros to ease the access to the fields of the structs
// Note that X must be a pointer
#define year(X) 	((X)->data[2])
#define month(X) 	((X)->data[1])
#define day(X) 		((X)->data[0])
#define hour(X) 	((X)->data[3])
#define duration(X) ((X)->data[4])
#define id(X) 		((X)->id)
#define user(X) 	((X)->user)
#define description(X) 	((X)->description)
#define type(X) 	((X)->type)

/**
 * Print an event
 *   text - prefix text
 *   event - the event to print 
 */
void print_event(char* text, Event* event);

/**
 * Print a list of events
 *   eventList - the list of events
 */
void print_event_list(EventList* eventlist);

/**
 * Create a list of events
 *   nEvents - the length of the list
 */
EventList* new_event_list(int nEvents);

/**
 * Destroy a list of events
 *   eventList - the list of events
 */
void destroy_event_list(EventList *);


// Validation functions

int validate_year(short year);
int validate_month(short month);
int validate_day(short day);
int validate_hour(short hour);
int validate_duration(short duration);
int validate_user(char* user);
int validate_description(char* description);

#endif // _FSO_EVENT_H_
