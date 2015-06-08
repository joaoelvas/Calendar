// This is an Academic Project, and was published after finishing the lecture.
// @author Joao Elvas @ FCT/UNL

#include <sqlite3.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "calendar.h"

static  sqlite3* calendar;

static char* error_msg = "";

static int nEvent;


// Callbacks
static int get_int(void *var, int argc, char **argv, char **azColName) {    
	* (int *) var = atoi(argv[0]);
   return 0;
}

static int get_event(void *event, int argc, char **argv, char **azColName){
	Event* ev = (Event *) event;
	
	id(ev) = atoi(argv[0]);
	strcpy(user(ev), argv[1]);
	strcpy(description(ev), argv[2]);
	year(ev) = atoi(argv[3]);
	month(ev) = atoi(argv[4]);
	day(ev) = atoi(argv[5]);
	hour(ev) = atoi(argv[6]);
	duration(ev) = atoi(argv[11]);

   return 0;
}

static int get_eventList(void *eventList, int argc, char **argv, char **azColName){
	return get_event(&((EventList *) eventList)->events[nEvent++], argc, argv, azColName);
}


// Calendar Open
int calendar_open() {
	return calendar_open_with_name(DEFAULT_CALENDAR_NAME);
}

int calendar_open_with_name (char *filename) {	
	int res;
	res = sqlite3_open_v2(filename, &calendar, SQLITE_OPEN_READWRITE, NULL);
	
	if (res != SQLITE_OK) {
      error_msg =  (char *) sqlite3_errmsg(calendar);
      return -1;
    }
    
    printf("Calendar opened successfully\n");
    return 0;
}

// Calendar Close
int calendar_close() {
	sqlite3_close(calendar);
	printf("Calendar closed\n");
	return 0;
}

// Calendar Create
int new_calendar() {
	return new_calendar_with_name(DEFAULT_CALENDAR_NAME);
}

int new_calendar_with_name(char *filename) {
	// Create database
	
	truncate(filename, 0);
	
	int res;
	res = sqlite3_open(filename, &calendar);
	if (res != SQLITE_OK) {
      error_msg =  (char *) sqlite3_errmsg(calendar);
      return -1;
    }
    
    // Create tables
    
    char *table = "CREATE TABLE IF NOT EXISTS events("  \
         "id 			INTEGER PRIMARY KEY  NOT NULL," \
         "owner			TEXT   	NOT NULL," \
         "description	TEXT   	NOT NULL," \
         "year         	INTEGER NOT NULL," \
         "month         INTEGER NOT NULL," \
         "day         	INTEGER NOT NULL," \
         "hour         	INTEGER NOT NULL," \
         "end_year         	INTEGER NOT NULL," \
         "end_month         INTEGER NOT NULL," \
         "end_day         	INTEGER NOT NULL," \
         "end_hour         	INTEGER NOT NULL," \
         "duration      INTEGER NOT NULL);";
    res = sqlite3_exec(calendar, table, 0, 0, &error_msg);
    if (res != SQLITE_OK)
    	return -1;
    
    table = "CREATE TABLE IF NOT EXISTS users("  \
         "login	TEXT   PRIMARY KEY	NOT NULL);";
    res = sqlite3_exec(calendar, table, 0, 0, &error_msg);
    if (res != SQLITE_OK)
    	return -1;
    	
    table = "CREATE TABLE IF NOT EXISTS subscribers("  \
         "id	INTEGER NOT NULL," \
         "user	TEXT   	NOT NULL);";
    res = sqlite3_exec(calendar, table, 0, 0, &error_msg);
    if (res != SQLITE_OK)
    	return -1;
    
	printf("Calendar created successfully\n");
    return 0;
}

// Calendar add 
int calendar_add(Event*  event) {
	char sql[500];
	int res;

	unsigned short end_hour = (hour(event) + duration(event)) % 2400;
	unsigned short aux_hour = (hour(event) + duration(event)) / 2400;
	
	unsigned short end_day = day(event);
	unsigned short end_month = month(event);
	unsigned short end_year = year(event);		
	
	if (aux_hour) { 
		// For simplicity's sake the following code assumes that all months with 30 days
		// If you want you can modify it to take into consideration the duration of each month
			
		unsigned short aux_day = (end_day + aux_hour) / 30; 
		end_day = (end_day + aux_hour) % 30; 
		
		if (aux_day) {
			unsigned short aux_month = (end_month + aux_day) / 12; 
			end_month = (end_month + aux_day) % 12; 
		
			if (aux_month)
				end_year = end_year + aux_month;
		}			
	} 
	
	sprintf(sql, "INSERT INTO events(owner, description, year, month, day, hour, " \
		"end_year, end_month, end_day, end_hour, duration)" \
		"VALUES ('%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d);" \
		"INSERT OR IGNORE INTO users(login)" \
		"VALUES ('%s');",
		user(event), description(event), year(event), month(event), day(event), hour(event), 
		end_year, end_month, end_day, end_hour, 
		duration(event), user(event));
	
	res = sqlite3_exec(calendar, sql, 0, 0, &error_msg);
    if (res != SQLITE_OK) {
    	return -1;
    }
    	
    sprintf(sql, "SELECT id from events WHERE " \
		"owner = '%s' AND " \
		"description = '%s' AND " \
    	"year = %d AND " \
    	"month = %d AND " \
    	"day = %d AND " \
    	"hour = %d AND " \
    	"end_year = %d AND " \
    	"end_month = %d AND " \
    	"end_day = %d AND " \
    	"end_hour = %d AND " \
    	"duration = %d;",
    	user(event), description(event), year(event), month(event), day(event), hour(event), 
    	end_year, end_month, end_day, end_hour, duration(event));

	int id = 0;
	res = sqlite3_exec(calendar, sql, get_int, &id, &error_msg);
    if (res != SQLITE_OK) {
    	return -1;
    }
    	    				    			
    sprintf(sql, "INSERT INTO subscribers(id, user)" \
		"VALUES (%d, '%s');", 
		id, user(event));
	
	res = sqlite3_exec(calendar, sql, 0, 0, &error_msg);
	  
    if (res != SQLITE_OK) 
    	return -1;
    
    id(event) = id;
    
    print_event("Added new event", event);
    printf ("  End Date %d/%d/%d  End Hour %d\n", end_day, end_month, end_year, end_hour);
    return 0;
}

EventList* calendar_list() {
	int res;
	int nEvents = 0;
	
    res = sqlite3_exec(calendar, "SELECT COUNT(*) from events", get_int, &nEvents, &error_msg);
    if (res != SQLITE_OK) {	
    	return NULL;
    }	
        	 		    
    EventList* eventList = new_event_list(nEvents);
    nEvent = 0; // required for the call function
    
    res = sqlite3_exec(calendar, "SELECT * from events", get_eventList, eventList, &error_msg);
        
    if (res != SQLITE_OK) {
	    destroy_event_list(eventList);
    	return NULL;	
    }	
    return eventList;
}

EventList* calendar_list_event(unsigned int id) {
	char sql[100];
	int res;
    	    
    EventList* eventList = new_event_list(1);
   	nEvent = 0; // required for the call function
    
    sprintf(sql, "SELECT * from events WHERE id = %d;", id);
    	
    res = sqlite3_exec(calendar, sql, get_eventList, eventList, &error_msg);
    
    if (res != SQLITE_OK) {
    	destroy_event_list(eventList);
    	return NULL;	
   	 }
   	 
    if (nEvent == 0) {
    	destroy_event_list(eventList);
   		printf ("List: event %d does not exist\n", id);
   		return NULL;
   	}
   		
    return eventList;
}

EventList* calendar_list_ongoing(unsigned short day, unsigned short month, unsigned short year, unsigned short hour) {
	char sql[500];
	
	printf("List ongoing events - current time: %d/%d/%d %d \n", day, month, year, hour	);	
									
	sprintf(sql, "SELECT COUNT(*) from events WHERE " \
    	"year <= %d AND " \
    	"month <= %d AND " \
    	"day <= %d AND " \
    	"hour <= %d AND "	 \
    	"end_year >= %d AND " \
    	"end_month >= %d AND " \
    	"end_day >= %d AND " \
    	"end_hour >= %d;",
    	year, month, day, hour, year, month, day, hour);
    	
    int res;	
	int nEvents;   
	
    res = sqlite3_exec(calendar, sql, get_int, &nEvents, &error_msg);
    if (res != SQLITE_OK) {
    	return NULL;
    }
    	
    EventList* eventList = new_event_list(nEvents);
    nEvent = 0;
    
    sprintf(sql, "SELECT * from events WHERE " \
		"year <= %d AND " \
    	"month <= %d AND " \
    	"day <= %d AND " \
    	"hour <= %d AND " \
    	"end_year >= %d AND " \
    	"end_month >= %d AND " \
    	"end_day >= %d AND " \
    	"end_hour >= %d;",
    	year, month, day, hour, year, month, day, hour);
    	
    res = sqlite3_exec(calendar, sql, get_eventList, eventList, &error_msg);
    
    if (res != SQLITE_OK)
    	return NULL;	
   	    	
    return eventList;
}

int calendar_remove(unsigned int id) {
	char sql[100];
	int res;
    
    Event event;
    event.id = 0;
    
    sprintf(sql, "SELECT * from events WHERE id = %d;", id);
    
    res = sqlite3_exec(calendar, sql, get_event, &event, &error_msg);
    if (res != SQLITE_OK) {
    	return -1;	
    }
   	
   	if (event.id == 0) {
   		printf ("Remove: event %d does not exist\n", id);
    	return -1;
   	}
    	
   	sprintf(sql, "DELETE from events WHERE id = %d;" \
    	"DELETE from subscribers WHERE id = %d;", 
    	id, id);
	res = sqlite3_exec(calendar, sql, 0, 0, &error_msg);
		
    if (res != SQLITE_OK) {
    	printf ("Remove: %s\n", error_msg);
    	return -1;
    }
    
    print_event("Removed event", &event);				    			
    
    return 0;
}


const char* calendar_error_msg() {
	return error_msg;
}


