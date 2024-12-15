
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parking.h"
#include <stdbool.h>
#include <gtk/gtk.h>
#include "worker.h"





int assign_worker_to_parking(const char *filename, const char *worker_id, const char *parking_id) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return 0; 
    }

    FILE *temp_file = fopen("workerdata_temp.txt", "w");
    if (!temp_file) {
        fclose(file);
        return 0; 
    }

    char buffer[100];
    int found = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        char id[20], name[50], current_park[50];
        
        // read file
        if (sscanf(buffer, "%s %s %s", id, name, current_park) == 3) {
            // If the worker_id == saisie change
            if (strcmp(id, worker_id) == 0) {
                fprintf(temp_file, "%s %s %s\n", id, name, parking_id);
                found = 1; // found
            } else {
                // just copy 
                fprintf(temp_file, "%s %s %s\n", id, name, current_park);
            }
        }
    }

    fclose(file);
    fclose(temp_file);

    // remplacer original avec temp
    if (found) {
        remove(filename);
        rename("workerdata_temp.txt", filename);
        return 1; // done
    } else {
        remove("workerdata_temp.txt");
        return 0; // worker id not found
    }
}
int unassign_worker(const char *filename, const char *worker_id) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return 0; 
    }

    FILE *temp_file = fopen("workerdata_temp.txt", "w");
    if (!temp_file) {
        fclose(file);
        return 0; 
    }

    char buffer[100];
    int found = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        worker w;

        // read  worker data 
        if (sscanf(buffer, "%s %s %s", w.workerid, w.workername, w.workerparkid) == 3) {
            //  unassign them
            if (strcmp(w.workerid, worker_id) == 0) {
                strcpy(w.workerparkid, "UNASSIGNED");
                found = 1; //  found and unassigned
            }
            // write  data  temp file
            fprintf(temp_file, "%s %s %s\n", w.workerid, w.workername, w.workerparkid);
        }
    }

    fclose(file);
    fclose(temp_file);

    // replace original file with  temp 
    if (found) {
        remove(filename);
        rename("workerdata_temp.txt", filename);
        return 1; // done
    } else {
        remove("workerdata_temp.txt");
        return 0; // worker id not found
    }
}

int is_park_id_exist(const char *filename, const char *park_id) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open %s\n", filename);
        return 0;
    }

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file)) {
        char id[20];
        if (sscanf(buffer, "%s", id) == 1 && strcmp(id, park_id) == 0) {
            fclose(file);
            return 1; //  id found
        }
    }

    fclose(file);
    return 0; // id not found
}
void parkingremoved(const char *filename, const char *park_id) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        g_print("Error: Could not open file %s.\n", filename);
        return;
    }

    worker w;
    int test = 0; 

    
    while (fscanf(file, "%s %s %s", w.workerid, w.workername, w.workerparkid) == 3) {
       
        if (strcmp(w.workerparkid, park_id) == 0) {
            test = unassign_worker(filename, w.workerid); 
         
        }
    }

    fclose(file);
}
int searchworker_by_id(const char *filename, const char *worker_id) {
    worker w;
    FILE *file = fopen(filename, "r");
    FILE *output_file = fopen("searchworker.txt", "w");

    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for reading.\n", filename);
        return -2; // Input file open failure
    }

    if (!output_file) {
        fprintf(stderr, "Error: Could not open file searchworker.txt for writing.\n");
        fclose(file);
        return -3; // Output file open failure
    }

    int found = 0; // Flag to indicate if worker is found
    while (fscanf(file, "%s %s %s", w.workerid, w.workername, w.workerparkid) == 3) {
        if (strcmp(w.workerid, worker_id) == 0) {
            // Write the matching worker to the output file
            fprintf(output_file, " %s %s %s\n",
                    w.workerid, w.workername, w.workerparkid);
            found = 1;
            break;
        }
    }

    fclose(file);
    fclose(output_file);

    return found ? 1 : 0; // Return 1 if found, otherwise 0
}

