#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parking.h"
#include <stdbool.h>
#include <gtk/gtk.h>
#include <time.h>

#define MAX_PARKINGS 100




// Function to check if a parking ID already exists in the file
int is_parking_id_unique(const char *filename, const char *park_id) {
    FILE *file = fopen(filename, "r");
    if (!file) return 1; // If file doesn't exist, the ID is unique

    char existing_id[20], status[20], size[20], location[50];
    while (fscanf(file, "%19s %19s %19s %49s", existing_id, status, size, location) == 4) {
        if (strcmp(existing_id, park_id) == 0) {
            fclose(file);
            return 0; // ID is not unique
        }
    }

    fclose(file);
    return 1; // ID is unique
}
// Function to generate a random parking ID
void generate_random_parking_id(char *park_id) {
    // Generate a random number between 0000 and 9999
    int random_number = rand() % 10000;
    // Format it as P####
    sprintf(park_id, "P%04d", random_number);
}

// Function to write a Parking entry to a file
void write_parking_to_file(const char *filename, Parking parking) {
    // Seed the random number generator
    srand(time(NULL));

    // Generate a unique parking ID
    do {
        generate_random_parking_id(parking.park_id);
    } while (!is_parking_id_unique(filename, parking.park_id));

    // Open the file in append mode
    FILE *file = fopen(filename, "a");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Write the parking data in a clean format
    fprintf(file, "%s %s %s %s\n", parking.park_id, parking.status, parking.size, parking.location);

    fclose(file);
}

/* read parking data and count the 
int read_parking_data(const char *filename, Parking parkings[], int max_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return 0;
    }

    int count = 0;
    while (count < max_count) {
        Parking parking;
        if (fscanf(file, "%s %s %s %s", parking.park_id, parking.status, parking.size, parking.location) != 4) {
            break; // Stop reading if format is incorrect or EOF is reached
        }
        parkings[count++] = parking;
    }

    fclose(file);
    return count;
}

void write_all_parking_data(const char *filename, Parking parkings[], int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %s %s %s\n", parkings[i].park_id, parkings[i].status, parkings[i].size, parkings[i].location);
    }

    fclose(file);
}*/

int remove_parking(const char *filename, const char *park_id) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        g_print("Error: Could not open file %s for reading.\n", filename);
        return 0;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        g_print("Error: Could not open temporary file for writing.\n");
        fclose(file);
        return 0;
    }

    Parking parking;
    bool found = false;

    // Read each parking 
    while (fscanf(file, "%s %s %s %s",parking.park_id,parking.status,parking.size, parking.location) == 4) {
        if (strcmp(parking.park_id, park_id) == 0) {
            found = true; // Mark as found 
        } else {
            fprintf(temp_file, "%s %s %s %s\n", parking.park_id,parking.status,parking.size,parking.location);
        }
    }

    fclose(file);
    fclose(temp_file);

    if (found) {
        // Replace the original 
        remove(filename);
        rename("temp.txt", filename);
        g_print("Parking with ID %s removed successfully.\n", park_id);
        return 1;
    } else {
        // Cleanup temp 
        remove("temp.txt");
        g_print("Error: Parking with ID %s not found.\n", park_id);
        return 0;
    }
}
int modify_parking(const char *filename, const char *park_id, const char *status, const char *size, const char *location) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        g_print("Error: Could not open file %s for reading.\n", filename);
        return 1;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        g_print("Error: Could not open temporary file for writing.\n");
        fclose(file);
        return 1;
    }

    char current_park_id[20], current_status[20], current_size[20], current_location[50];
    bool found = false;

    while (fscanf(file, "%s %s %s %s", current_park_id, current_status, current_size, current_location) == 4) {
        if (strcmp(current_park_id, park_id) == 0) {
            // modify 
            found = true;
            fprintf(temp_file, "%s %s %s %s\n", park_id, status, size, location);
        } else {
            // write the existing 
            fprintf(temp_file, "%s %s %s %s\n", current_park_id, current_status, current_size, current_location);
        }
    }

    fclose(file);
    fclose(temp_file);

    if (found) {
        
        
        rename("temp.txt", filename); 
        g_print("Parking with ID %s modified successfully.\n", park_id);
        return 0;
        
    } else {
        
        remove("temp.txt");
        g_print("Error: Parking with ID %s not found.\n", park_id);
        return 1;
    }
}


int search_parking_by_id(const char *filename, const char *park_id) {
    Parking parking;
    FILE *file = fopen(filename, "r");
    FILE *output_file = fopen("searchpark.txt", "w");

    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for reading.\n", filename);
        return -2; // File open failure
    }

    if (!output_file) {
        fprintf(stderr, "Error: Could not open file searchpark.txt for writing.\n");
        fclose(file);
        return -3; // Output file open failure
    }

    int found = 0; // Flag to indicate if parking is found
    while (fscanf(file, "%s %s %s %s", 
                  parking.park_id, parking.status, parking.size, parking.location) == 4) {
        if (strcmp(parking.park_id, park_id) == 0) {
            // Write the matching parking to the output file
            fprintf(output_file, "%s %s %s %s", 
                    parking.park_id, parking.status, parking.size, parking.location);
            found = 1;
            break;
        }
    }

    fclose(file);
    fclose(output_file);

    return found ? 1 : 0; // Return 1 if found, otherwise 0
}

