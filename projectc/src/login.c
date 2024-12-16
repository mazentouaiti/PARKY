#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "login.h"
#include <stdbool.h>
#include <gtk/gtk.h>
#include <time.h>



// Function to create an account and save it to a file
void create_account(const char *filename, utilisateur u) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for appending.\n", filename);
        return; // Gracefully handle the error
    }

    fprintf(file, "%s %s %s %s %s %s\n", u.name, u.surname, u.login, u.pass, u.mail, u.sex);

    fclose(file);
    
}


void sex(int valeur, char *msg) {
    if (valeur == 1) {
        strcpy(msg, "Female");
    } else if (valeur == 2) {
        strcpy(msg, "Male");
    } else {
        strcpy(msg, "Unknown");
    }
}
int log_in(const char *filename, const char *login, const char *pass) {
    utilisateur u;
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for reading.\n", filename);
        return -2; // Return -2 to indicate file open failure
    }

    while (fscanf(file, "%49s %49s %49s %49s %99s %9s\n", u.name, u.surname, u.login, u.pass, u.mail, u.sex) == 6) {
        if (strcmp(login, u.login) == 0) {
            fclose(file);
            if (strcmp(pass, u.pass) == 0) {
                return 1; // Login and password match
            } else {
                return 0; // Password incorrect
            }
        }
    }

    fclose(file);
    return -1; // Login not found
}


