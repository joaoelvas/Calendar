// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL

#ifndef _FSO_CALENDAR_H_
#define _FSO_CALENDAR_H_

#include "event.h"

#define DEFAULT_CALENDAR_NAME "myCalendar.db"

/**
 * Create a new calendar with default name
 */
int new_calendar();

/**
 * Create a new calendar with given name
 */
int new_calendar_with_name(char *filename); 

/**
 * Open existing calendar with default name
 */
int calendar_open();

/**
 * Open existing calendar with given name
 */
int calendar_open_with_name(char *filename); 

/**
 * Close calendar
 */
int calendar_close();

/**
 * Add a new event to the calendar
 */ 
int calendar_add(Event*  event);

/**
 * Remove a particular event (with given id)
 */ 
int calendar_remove(unsigned int id);

/**
 * List all events in the calendar
 */ 
EventList* calendar_list();

/**
 * List a particular event (with given id)
 */ 
EventList* calendar_list_event(unsigned int id);

/**
 * List all events in progress at the given time 
 */ 
EventList* calendar_list_ongoing(unsigned short day, unsigned short month, unsigned short year, unsigned short hour);

/**
 * Retrieve last error message
 */ 
const char* calendar_error_msg();

#endif // _FSO_CALENDAR_H_
