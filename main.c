#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "csv.h"
#include "vaccine.h"

#define N 200

void vaccineData_print(tVaccineLotData data) {
    char buffer[1024];
    int i;
    if (vaccineData_len(data) > 0) {
        for(i = 0 ; i < vaccineData_len(data) ; i++) {
            vaccineData_get(data, i, buffer);
            printf("%d;%s\n", i, buffer);
        }
    }    
}


bool vaccineData_check(tVaccineLotData data, int len, const char** reference) {
    bool result = true;
    char buffer[1024];
    int i;
    
    if (vaccineData_len(data) != len) {
        result = false;
    }
    if (result) {    
        for(i = 0 ; i < len ; i++) {
            vaccineData_get(data, i, buffer);
            if (strcmp(buffer, reference[i]) != 0) {
                result = false;
            }
        }
    }   
    return result;
}


int main(int argc, char **argv)
{       
    bool failed;    
    tCSVData test_data;      
    tCSVEntry entry;
    tVaccineLotData vaccineLotData;
    tVaccineLot vaccineLot;
    tDateTime dateTime;   
    char lot_data[1024];
    int day;
    int month;
    int initial_size;
    int i;
    
    // Define test data
    const char* test_data_str = "01/08/2023;13:45;CA08000;PFIZER;2;21;300\n" \
                                "01/09/2023;11:00;CA08018;MODERNA;1;0;150\n" \
                                "01/10/2023;17:15;CA25001;PFIZER;2;21;100\n";
    const char* test_data2_str = "01/08/2023;13:45;CA08000;PFIZER;2;21;50\n" \
                                 "01/10/2023;17:15;CA25001;PFIZER;2;21;250\n" \
                                 "01/09/2023;11:00;CA08018;MODERNA;1;0;200\n";
                                 
    // Define the test reference data
    int ref1_len = 3;
    const char* ref1[] = {"01/08/2023;13:45;CA08000;PFIZER;2;21;300", "01/09/2023;11:00;CA08018;MODERNA;1;0;150", "01/10/2023;17:15;CA25001;PFIZER;2;21;100"};

    int ref2_len = 3;
    const char* ref2[] = {"01/08/2023;13:45;CA08000;PFIZER;2;21;350", "01/09/2023;11:00;CA08018;MODERNA;1;0;350", "01/10/2023;17:15;CA25001;PFIZER;2;21;350"};
    
    int ref3_len = 3;
    const char* ref3[] = {"01/08/2023;13:45;CA08000;PFIZER;2;21;350", "01/09/2023;11:00;CA08018;MODERNA;1;0;300", "01/10/2023;17:15;CA25001;PFIZER;2;21;350"};
    
    int ref4_len = 2;    
    const char* ref4[] = {"01/08/2023;13:45;CA08000;PFIZER;2;21;350", "01/10/2023;17:15;CA25001;PFIZER;2;21;350"};    
         
                                
    // Read the data in CSV format
    csv_init(&test_data);
    csv_parse(&test_data, test_data_str, "VACCINE_LOT");
    
      
    // Initialize the vaccine data
    vaccineData_init(&vaccineLotData);
            
    // Add data
    for(i=0; i < csv_numEntries(test_data); i++) {
        // Initialize the position
        vaccine_parse(&vaccineLot, csv_getEntry(test_data, i)[0]);
        
        // Add new vaccine batch the list of vaccines
        vaccineData_add(&vaccineLotData, vaccineLot);
    }
    failed = !vaccineData_check(vaccineLotData, ref1_len, ref1);
        
    // Remove old data
    csv_free(&test_data);
    
    // List current lots
    vaccineData_print(vaccineLotData);
    printf("\n");
   
   
    // Load new data
    csv_init(&test_data);
    csv_parse(&test_data, test_data2_str, "VACCINE_LOT");
    
    // Add new data
    for(i=0; i < csv_numEntries(test_data); i++) {
        // Initialize the position
        vaccine_parse(&vaccineLot, csv_getEntry(test_data, i)[0]);
        
        // Add new vaccine batch the list of vaccines
        vaccineData_add(&vaccineLotData, vaccineLot);
    }
    
    failed = !vaccineData_check(vaccineLotData, ref2_len, ref2);
    
    // Remove old data
    csv_free(&test_data);
    
    // List current lots
    vaccineData_print(vaccineLotData);
    printf("\n");
    
    
    // Remove some doses
    dateTime_parse(&dateTime, "01/09/2023", "11:00");
    vaccineData_del(&vaccineLotData, "CA08018", "MODERNA", dateTime, 50);
    
    failed = !vaccineData_check(vaccineLotData, ref3_len, ref3);
    
    // List current lots
    vaccineData_print(vaccineLotData);
    printf("\n");
       
    // Remove remaining doses    
    vaccineData_del(&vaccineLotData, "CA08018", "MODERNA", dateTime, 300);
    
    failed = !vaccineData_check(vaccineLotData, ref4_len, ref4);
    
    // List current lots
    vaccineData_print(vaccineLotData);
    printf("\n");
    
    // Remove not existing registry (the same, as now it should not exist)
    vaccineData_del(&vaccineLotData, "CA08018", "MODERNA", dateTime, 1000);
    
    failed = !vaccineData_check(vaccineLotData, ref4_len, ref4);
    
    // List current lots
    vaccineData_print(vaccineLotData);
    printf("\n");
        
    day = 1;
    month = 1;
    initial_size = vaccineData_len(vaccineLotData);
    for(i=0; i < N && !failed; i++) {    
        csv_initEntry(&entry);
        
        // Create a lot data
        sprintf(lot_data, "%02d/%02d/2024;13:45;CA08018;PFIZER;2;21;300", day, month);
        csv_parseEntry(&entry, lot_data, "VACCINE_LOT");
        
        // Parse the vaccine lot data
        vaccine_parse(&vaccineLot, entry);
        
        // Add new vaccine batch the list of vaccines
        vaccineData_add(&vaccineLotData, vaccineLot);
        
        // Update date (avoid invalid dates)
        day ++;
        if (day > 28) {
            day = 1;
            month += 1;
        }
        
        // Check the new size
        if (vaccineData_len(vaccineLotData) != initial_size + i + 1) {
            failed = true;
        }
        
        csv_freeEntry(&entry);
    }   
    
    // Show result
    if (failed) {
        printf("ERROR!!! adding new %d vaccine lots\n", N);
    } else {
        printf("Added new %d vaccine lots\n", N);
    }
    printf("\n");
        
    // Remove all data
    vaccineData_free(&vaccineLotData);    
    
    failed = vaccineData_len(vaccineLotData) != 0;
        
    if (!failed) {
        printf("No vaccine lots\n");
    } else {
        printf("ERROR!!! there are still remaining vaccines\n");
    }
        
    // Wait user to press a key to ensure results are shown
    printf("Press key to continue...");
    getchar();
    
	exit(EXIT_SUCCESS);
}




