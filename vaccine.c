#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "vaccine.h"

// Parse a tDateTime from string information
void dateTime_parse(tDateTime* dateTime, const char* date, const char* time) {
    // Check output data
    assert(dateTime != NULL);
    
    // Check input date
    assert(date != NULL);
    assert(strlen(date) == 10);
    
    // Check input time
    assert(time != NULL);
    assert(strlen(time) == 5);
    
    // Parse the input date
    sscanf(date, "%d/%d/%d", &(dateTime->date.day), &(dateTime->date.month), &(dateTime->date.year));
    
    // Parse the input time
    sscanf(time, "%d:%d", &(dateTime->time.hour), &(dateTime->time.minutes));
}

// Compare two tDateTime structures and return true if they contain the same value or false otherwise.
bool dateTime_equals(tDateTime dateTime1, tDateTime dateTime2) {
    if (dateTime1.date.day != dateTime2.date.day || dateTime1.date.month != dateTime2.date.month || dateTime1.date.year != dateTime2.date.year) {
        return false;
    }
    
    return dateTime1.time.hour == dateTime2.time.hour && dateTime1.time.minutes == dateTime2.time.minutes;
}

// EX2: Implement your methods here....

// Initialize the vaccine lots data
void vaccineData_init(tVaccineLotData* data) {
    // TODO
    
    /////////////
    // Set the initial number of elements to zero.
    data->count = 0;
    /////////////
}

// Get the number of lots
int vaccineData_len(tVaccineLotData data) {
    //return -1;
    
    //////////////
    // Return the number of elements
    return data.count;
    //////////////
}

// Get a lot of vaccines
void vaccineData_get(tVaccineLotData data, int index, char* buffer) {
    // TODO
    
    /////////////
    assert(index < data.count);
    sprintf(buffer, "%02d/%02d/%04d;%02d:%02d;%s;%s;%d;%d;%d", 
        data.elems[index].timestamp.date.day, data.elems[index].timestamp.date.month, data.elems[index].timestamp.date.year,
        data.elems[index].timestamp.time.hour, data.elems[index].timestamp.time.minutes,
        data.elems[index].hcc,
        data.elems[index].vaccine.name, data.elems[index].vaccine.required, data.elems[index].vaccine.days,
        data.elems[index].doses
    );
    /////////////
}

// Parse input from CSVEntry
void vaccine_parse(tVaccineLot* data, tCSVEntry entry) {
    // TODO
    
    /////////////
    char date[11];
    char time[6];
    
    // Check input data (Pre-conditions)
    assert(data != NULL);    
    assert(csv_numFields(entry) == 7);
    
    // Get the date and time
    csv_getAsString(entry, 0, date, 11);    
    csv_getAsString(entry, 1, time, 6);
    dateTime_parse(&(data->timestamp), date, time);
    
    // Assign the center code
    csv_getAsString(entry, 2, data->hcc, MAX_HCC_LEN + 1);
        
    // Assign the vaccine data
    csv_getAsString(entry, 3, data->vaccine.name, MAX_VACCINE_NAME_LEN + 1);
    data->vaccine.required = csv_getAsInteger(entry, 4);
    data->vaccine.days = csv_getAsInteger(entry, 5);
    
    // Set the number of doses
    data->doses = csv_getAsInteger(entry, 6);
    /////////////
}

////////////////////////////////////////

// Add a new vaccine lot
void vaccineData_add(tVaccineLotData* data, tVaccineLot lot) {
    int idx = -1;
    // Check input data (Pre-conditions)
    assert(data != NULL);    
    
    // Check if an entry with this data already exists
    idx = vaccineData_find(*data, lot.hcc, lot.vaccine.name, lot.timestamp);
    
    // If it does not exist, create a new entry, otherwise add the number of doses
    if (idx < 0) {
        assert(data->count < MAX_LOTS);
        vaccineLot_cpy(&(data->elems[data->count]), lot);
        data->count ++;        
    } else {
        data->elems[idx].doses += lot.doses;
    }
}

// Remove vaccines from a lot
void vaccineData_del(tVaccineLotData* data, const char* hcc, const char* vaccine, tDateTime timestamp, int doses) {
    int idx;
    int i;
    
    // Check if an entry with this data already exists
    idx = vaccineData_find(*data, hcc, vaccine, timestamp);
    
    if (idx >= 0) {
        // Reduce the number of doses
        data->elems[idx].doses -= doses;
        // Shift elements to remove selected
        if (data->elems[idx].doses <= 0) {
            for(i = idx; i < data->count-1; i++) {
                // Copy element on position i+1 to position i
                vaccineLot_cpy(&(data->elems[i]), data->elems[i+1]);
            }
            // Update the number of elements
            data->count--;     
        }
    }
}

// [AUX METHOD] Return the position of a vaccine lot entry with provided information. -1 if it does not exist
int vaccineData_find(tVaccineLotData data, const char* hcc, const char* vaccine, tDateTime timestamp) {
    int i;
    int res = -1;

    i = 0;
    while ((i < data.count) && (res < 0)) {
        if((strcmp(data.elems[i].hcc, hcc) == 0) && (strcmp(data.elems[i].vaccine.name, vaccine) == 0) && dateTime_equals(data.elems[i].timestamp, timestamp)) {
            res = i;
        }
        else {
            i++;
        } 
    }
    
    return res;
}

// [AUX METHODS] Copy the data from the source to destination
void vaccine_cpy(tVaccine* destination, tVaccine source) {
    destination->required = source.required;
    destination->days = source.days;
    strcpy(destination->name, source.name);
}
void vaccineLot_cpy(tVaccineLot* destination, tVaccineLot source) {
    strcpy(destination->hcc, source.hcc);
    destination->doses = source.doses;
    destination->timestamp = source.timestamp;
    vaccine_cpy(&(destination->vaccine), source.vaccine);
}
