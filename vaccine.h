#ifndef __VACCINE_H__
#define __VACCINE_H__
#include "csv.h"

typedef struct _tDate {    
    int day; 
    int month;
    int year;
} tDate;

typedef struct _tTime {
    int hour; 
    int minutes;
} tTime;

typedef struct _tDateTime {
    tDate date;
    tTime time;    
} tDateTime;


// Parse a tDateTime from string information
void dateTime_parse(tDateTime* dateTime, const char* date, const char* time);

// Compare two tDateTime structures and return true if they contain the same value or false otherwise.
bool dateTime_equals(tDateTime dateTime1, tDateTime dateTime2);

// Ex 1: Define data types here...

///////////////////////////
#define MAX_LOTS 150
// Maximum length of vaccine name
#define MAX_VACCINE_NAME_LEN 15
// Maximum length of health center code
#define MAX_HCC_LEN 7

typedef struct _tVaccine {
    char name[MAX_VACCINE_NAME_LEN + 1];
    int required;
    int days;
} tVaccine;

typedef struct _tVaccineLot {
    tVaccine vaccine;
    tDateTime timestamp;
    char hcc[MAX_HCC_LEN + 1];
    int doses;
} tVaccineLot;

typedef struct _tVaccineLotData {
    tVaccineLot elems[MAX_LOTS];
    int count;
} tVaccineLotData;

//////////////////////////////////

// Ex 2: Define your methods here ....

// Initialize the vaccine lots data
void vaccineData_init(tVaccineLotData* data);

// Get the number of lots
int vaccineData_len(tVaccineLotData data);

// Get a lot of vaccines
void vaccineData_get(tVaccineLotData data, int index, char* buffer);

// Parse input from CSVEntry
void vaccine_parse(tVaccineLot* data, tCSVEntry entry);

// ...

////////////////////////////////////////

// Add a new vaccine lot
void vaccineData_add(tVaccineLotData* data, tVaccineLot lot);

// Remove vaccines from a lot
void vaccineData_del(tVaccineLotData* data, const char* hcc, const char* vaccine, tDateTime timestamp, int doses);

// [AUX METHOD] Return the position of a vaccine lot entry with provided information. -1 if it does not exist
int vaccineData_find(tVaccineLotData data, const char* hcc, const char* vaccine, tDateTime timestamp);

// [AUX METHODS] Copy the data from the source to destination
void vaccine_cpy(tVaccine* destination, tVaccine source);
void vaccineLot_cpy(tVaccineLot* destination, tVaccineLot source);

////////////////////////////////////////////

#endif
