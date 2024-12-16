#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operation.h"
#include "global.h"

/*if (logged_in_citizen_id != -1) {
    // User is logged in; proceed with operations
    g_print("Current logged-in ID: %d\n", logged_in_citizen_id);
} else {
    // No user is logged in
    g_print("No user currently logged in.\n");
}*/

int add_citizen(const char *filename,citizen *citizen_data)
{
 FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    fprintf(file, "%d %s %s %s %s %s %s %.2f %s %s %s\n",
            citizen_data->id, citizen_data->name, citizen_data->surname,
            citizen_data->email, citizen_data->password, citizen_data->car_brand,
            citizen_data->car_serial, citizen_data->balance,
            citizen_data->color, citizen_data->gender,citizen_data->profile_picture);

    fclose(file);
    return 1;

}
int read_citizen_profile(const char *filename, citizen *citizen_data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    if (fscanf(file, "%d %s %s %s %s %s %s %f %s %s %s\n",
               &citizen_data->id, citizen_data->name, citizen_data->surname,
               citizen_data->email, citizen_data->password, citizen_data->car_brand,
               citizen_data->car_serial, &citizen_data->balance,
               citizen_data->color, citizen_data->gender,citizen_data->profile_picture) != 11) {
        fclose(file);
        return -2; 
    }

    fclose(file);
    return 0;
}
int read_citizen_profile_by_id(const char *filename, int id_to_search, citizen *citizen_data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Error opening the file
    }

    while (fscanf(file, "%d %s %s %s %s %s %s %f %s %s %s\n", 
                   &citizen_data->id, citizen_data->name, citizen_data->surname,
               citizen_data->email, citizen_data->password, citizen_data->car_brand,
               citizen_data->car_serial, &citizen_data->balance,
               citizen_data->color, citizen_data->gender,citizen_data->profile_picture) != EOF) {
        if (citizen_data->id == id_to_search) {
            fclose(file);
            return 1; // Found the citizen
        }
    }

    fclose(file);
    return 0; // No citizen found
}


int search_all_citizens_by_id(const char *filename, int id_to_search, citizen *citizen_data_array, int max_results) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Unable to open file
    }

    int found_count = 0;
    while (fscanf(file, "%d %s %s %s %s %s %s %f %s %s %s\n", 
                   &citizen_data_array[found_count].id, 
                   citizen_data_array[found_count].name, 
                   citizen_data_array[found_count].surname,
                   citizen_data_array[found_count].email,
		   citizen_data_array[found_count].password,
                   citizen_data_array[found_count].car_brand,
                   citizen_data_array[found_count].car_serial,
		   &citizen_data_array[found_count].balance,
		   citizen_data_array[found_count].color,
		   citizen_data_array[found_count].gender,
		   citizen_data_array[found_count].profile_picture
                   ) != EOF) {
        if (citizen_data_array[found_count].id == id_to_search) {
            found_count++;
            if (found_count >= max_results) {
                break; // Stop if max results are found
            }
        }
    }
    
    fclose(file);

    if (found_count == 0) {
        return 0; // No citizen found
    }

    return found_count; // Return number of found citizens
}

int search_citizen_by_id(const char *filename, int id_to_search) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    int id;
    char name[50], surname[50], email[100], password[50];
    char car_brand[50], car_serial[50], color[50];
    float balance;
    char gender[10];
    char profile_picture[100];

    // Search for the citizen by ID
    while (fscanf(file, "%d %s %s %s %s %s %s %f %s %s %s\n",
                  &id, name, surname, email, password, car_brand,
                  car_serial, &balance, color, gender, profile_picture) == 11) {
        if (id == id_to_search) {
            fclose(file);
            return 1; // Found the citizen
        }
    }

    fclose(file);
    return 0; // Not found
}

int delete_citizen_profile(const char *filename, int id_to_delete) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        perror("Error opening temporary file");
        fclose(file);
        return -1;
    }

    int id;
    char name[50], surname[50], email[100], password[50];
    char car_brand[50], car_serial[50], color[50];
    float balance;
    char gender[10];
    char profile_picture[100];
    int found = 0;

    // Read each line and copy to temp file if ID doesn't match
    while (fscanf(file, "%d %s %s %s %s %s %s %f %s %s %s\n",
                  &id, name, surname, email, password, car_brand,
                  car_serial, &balance, color, gender, profile_picture) == 11) {
        if (id == id_to_delete) {
            found = 1; // Mark ID as found
            continue;  // Skip writing this line to the temp file (delete it)
        }
        // Write all other clients to temp file
        fprintf(temp_file, "%d %s %s %s %s %s %s %.2f %s %s %s\n",
                id, name, surname, email, password, car_brand,
                car_serial, balance, color, gender, profile_picture);
    }

    fclose(file);
    fclose(temp_file);

    // If the ID was found, replace the original file with the temp file
    if (found) {
        remove(filename);    // Delete the original file
        rename("temp.txt", filename); // Rename temp file to the original file
        return 0; // Success
    } else {
        remove("temp.txt"); // Delete temp file if ID wasn't found
        return -2; // ID not found
    }
}


int update_citizen_profile(const char *filename, const citizen *citizen_data) {
  if (logged_in_citizen_id == -1) {
        fprintf(stderr, "Error: No citizen is logged in.\n");
        return -1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file for reading");
        return -1;
    }

    // Temporary file to store updated data
    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        perror("Error opening temporary file for writing");
        fclose(file);
        return -1;
    }

    char line[512];
    int id_found = 0;

    // Process each line
    while (fgets(line, sizeof(line), file)) {
        citizen temp_citizen;

        // Parse the line
        sscanf(line, "%d %s %s %s %s %s %s %f %s %s %s",
               &temp_citizen.id, temp_citizen.name, temp_citizen.surname,
               temp_citizen.email, temp_citizen.password, temp_citizen.car_brand,
               temp_citizen.car_serial, &temp_citizen.balance,
               temp_citizen.color, temp_citizen.gender, temp_citizen.profile_picture);

        // Check if the ID matches the logged-in citizen's ID
        if (temp_citizen.id == logged_in_citizen_id) {
            id_found = 1;
            // Write the updated citizen data to the temp file
            fprintf(temp_file, "%d %s %s %s %s %s %s %.2f %s %s %s\n",
                    citizen_data->id, citizen_data->name, citizen_data->surname,
                    citizen_data->email, citizen_data->password, citizen_data->car_brand,
                    citizen_data->car_serial, citizen_data->balance,
                    citizen_data->color, citizen_data->gender, citizen_data->profile_picture);
        } else {
            // Write the original line to the temp file
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    // Replace the original file with the updated file
    if (id_found) {
        if (remove(filename) != 0 || rename("temp.txt", filename) != 0) {
            perror("Error replacing the original file");
            return -1;
        }
    } else {
        // If the logged-in ID was not found, clean up the temp file
        remove("temp.txt");
        fprintf(stderr, "Error: Logged-in citizen ID not found in the file\n");
        return 1; // ID not found
    }

    return 0; // Success

}

int read_parking_data(const char *filename, parking_data *data_array, int max_entries)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Indicate failure
    }

    int count = 0;
    while (count < max_entries && fscanf(file, "%49s %49s %f", 
                                         data_array[count].duration, 
                                         data_array[count].date, 
                                         &data_array[count].cost) == 3) {
        count++;
    }

    fclose(file);
    return count; // Return the number of records read
}


