// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "event.h"

/**
 * Print a list of events
 *   eventList - the list of events
 */
void print_event_list(EventList *eventList) {
	printf("Listing %d events.\n", eventList->nEvents);
	
	int i = 0;
	for ( ; i < eventList->nEvents; i++) 
		print_event("", &eventList->events[i]);
}

/**
 * Print an event
 *   text - prefix text
 *   event - the event to print 
 */
void print_event(char* text, Event* event) {
	float hour = ((float) hour(event))/100;

	printf("%s\n Id: %u\n  Date: %u/%u/%u  Hour: %.2f\n  Duration: %u minutes\n  Description: %s\n  Created by: %s\n",
	 text, id(event), day(event), month(event), year(event), hour, duration(event), 
	 description(event), user(event));
}

/**
 * Create a list of events
 *   nEvents - the length of the list
 */
EventList* new_event_list(int nEvents) {
	EventList* eventList = (EventList* ) malloc(sizeof(EventList));
    eventList->nEvents = nEvents;
    eventList->events = malloc(sizeof(Event) * nEvents);
    return eventList;
}

/**
 * Destroy a list of events
 *   eventList - the list of events
 */
void destroy_event_list(EventList *eventList) {
	free(eventList->events);
	free(eventList);
}


// Validation functions

int validate_user(char* user) {
	return (strlen(user) <= MAX_USER);
}

int validate_year(short year) {
	// NOT IMPLEMENTED
	return 1;
}	

int validate_month(short month)  {
	// NOT IMPLEMENTED
	return 1;
}	

int validate_day(short day)  {
	// NOT IMPLEMENTED
	return 1;	
}	

int validate_hour(short hour)  {
	// NOT IMPLEMENTED
	return 1;	
}	

int validate_duration(short duration)  {
	// NOT IMPLEMENTED
	return 1;	
}	

int validate_description(char* description)  {
	// NOT IMPLEMENTED
	return 1;	
}	