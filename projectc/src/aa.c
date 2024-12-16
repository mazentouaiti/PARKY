#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aa.h"
#include <gtk/gtk.h>



void set_label_message(GtkWidget *label_review, const char *message) {
    gtk_label_set_text(GTK_LABEL(label_review), message);
}


void edit_parking(const char *filename, const char *parking_id, ParkingEntry *updated_parking) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    ParkingEntry current_parking;
    int found = 0;
    char old_matricule[50] = ""; // To store the old matricule (vehicle ID)

    if (!file || !temp) {
        perror("Failed to open parking.txt or temp.txt");
        return;
    }

    // Read the file and process each record
    while (fscanf(file, "%49s %49s %d %49s %49s", 
                  current_parking.id, 
                  current_parking.entered_date, 
                  &current_parking.nbrspaces, 
                  current_parking.matricule, 
                  current_parking.phone) != EOF) {
        
        if (strcmp(current_parking.id, parking_id) == 0) {
            found = 1;
            // Store old matricule for later status update
            strcpy(old_matricule, current_parking.matricule);

            // Update fields only if they are not empty
            if (strlen(updated_parking->entered_date) > 0) 
                strcpy(current_parking.entered_date, updated_parking->entered_date);
            if (updated_parking->nbrspaces > 0) 
                current_parking.nbrspaces = updated_parking->nbrspaces;
            if (strlen(updated_parking->matricule) > 0) 
                strcpy(current_parking.matricule, updated_parking->matricule);
            if (strlen(updated_parking->phone) > 0) 
                strcpy(current_parking.phone, updated_parking->phone);
        }

        // Write the (potentially updated) parking entry record back to the temp file
        fprintf(temp, "%s\t%s\t%d\t%s\t%s\n", 
                current_parking.id, 
                current_parking.entered_date, 
                current_parking.nbrspaces, 
                current_parking.matricule, 
                current_parking.phone);
    }

    fclose(file);
    fclose(temp);

    // Replace the original file with the updated file
    remove(filename);
    rename("temp.txt", filename);
    
    // Update the label with a success message
    GtkWidget *label25ala;
    if (found) {
        set_label_message(label25ala, "Parking entry edited successfully!");
    } else {
        set_label_message(label25ala, "Edit failed! Parking ID not found.");
    }
    
    // Update the matricule status if it was changed
    if (found) {
        if (strlen(updated_parking->matricule) > 0 && strcmp(updated_parking->matricule, old_matricule) != 0) {
            // You can add logic to handle matricule updates, for example:
            // update_vehicle_status(updated_parking->matricule, 1); // Mark new matricule as assigned
            // update_vehicle_status(old_matricule, 0); // Mark old matricule as not assigned
        }
    } else {
        printf("Edit failed: Parking ID not found.\n");
    }
}
int validateDate(const char *date) {
    int day, month, year;
    if (sscanf(date, "%2d-%2d-%4d", &day, &month, &year) == 3) {
        if (month >= 1 && month <= 12) {
            // Optionnel : Ajouter la validation du jour et de l'année
            if (day >= 1 && day <= 31) {
                if (year > 1900) {  // Validation simple de l'année
                    return 1;  // Date valide
                }
            }
        }
    }
    return 0;  // Date invalide
}
int delete_parking(const char *filename, const char *parking_id) {
    ParkingEntry p_list[100]; // Assume max 100 parkings for simplicity
    int count = 0, found = 0;

    // Open file for reading
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open parking.txt");
        return 0;
    }

    // Read all parkings into memory
    while (fscanf(file, "%49s %49s %d %49s %49s", 
                  p_list[count].id, 
                  p_list[count].entered_date, 
                  &p_list[count].nbrspaces, 
                  p_list[count].matricule, 
                  p_list[count].phone) != EOF) {
        if (strcmp(p_list[count].id, parking_id) == 0) {
            found = 1; // Mark as found but don't increment count (skip this entry)
        } else {
            count++; // Add valid entries to the list
        }
    }
    fclose(file);

    if (!found) {
        printf("Delete failed! Parking ID not found.\n");
        return 0;
    }

    // Open file for writing to save updated list
    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to write to parking.txt");
        return 0;
    }

    // Write the remaining parking entries back to the file
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\t%s\t%d\t%s\t%s\n", 
                p_list[i].id, 
                p_list[i].entered_date, 
                p_list[i].nbrspaces, 
                p_list[i].matricule, 
                p_list[i].phone);
    }
    fclose(file);

    printf("Parking entry with ID %s deleted successfully!\n", parking_id);
    return 1;
}
void refresh_parking_treeview(GtkWidget *parking_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
         //Set up the columns (only once)
        setup_parking_treeview1(parking_treeview);
        columns_added = TRUE;
    }

     //Create a new ListStore and populate it with updated data
    GtkListStore *store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("data.txt", "r");
    if (!file) {
        perror("Failed to open data.txt");
        return;
    }

     //Variables to hold each field from the ParkingEntry structure
    char id[50], entered_date[50], nbrspaces[50], matricule[50], phone[50];
    
     //Read the file line by line and populate the TreeView
    while (fscanf(file, "%49s\t%49s\t%49s\t%49s\t%49s", id, entered_date, nbrspaces, matricule, phone) != EOF) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter); // Add a new row
        gtk_list_store_set(store, &iter,
                           0, id,
                           1, entered_date,
                           2, nbrspaces,
                           3, matricule,
                           4, phone,
                           -1);
    }
    fclose(file);

     //Attach the ListStore to the TreeView
     gtk_tree_view_set_model(GTK_TREE_VIEW(parking_treeview), GTK_TREE_MODEL(store));
     g_object_unref(store); // Release the ListStore reference
}

void setup_parking_treeview1(GtkWidget *parking_treeview) {
    // Column titles for the ParkingEntry structure
    const char *titles[] = {"ID", "Entered Date", "Nbr Spaces", "Matricule", "Phone"};
    
    // Create a TreeStore to store the ParkingEntry data
    GtkTreeStore *store = gtk_tree_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);

    // Here we add some dummy data to the tree store for demonstration purposes
    GtkTreeIter iter;
    ParkingEntry entry = {"123", "2024-12-13", 10, "ABC123", "555-1234"};
    
    gtk_tree_store_append(store, &iter, NULL);  // Append a row to the store
    gtk_tree_store_set(store, &iter, 
                       0, entry.id, 
                       1, entry.entered_date, 
                       2, entry.nbrspaces, 
                       3, entry.matricule, 
                       4, entry.phone, 
                       -1);  // Set values for the row
    
    // Set the model to the treeview
    gtk_tree_view_set_model(GTK_TREE_VIEW(parking_treeview), GTK_TREE_MODEL(store));

    // Create and append columns to the treeview for each field in ParkingEntry
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(parking_treeview), column);
    }

    // Unreference the model, as it's now owned by the tree view
    g_object_unref(store);
}
void bubble_sort_parkings(Parking1 *parkings, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (parkings[j].available_places < parkings[j + 1].available_places) {
                // Échanger les parkings
                Parking1 temp = parkings[j];
                parkings[j] = parkings[j + 1];
                parkings[j + 1] = temp;
            }
        }
    }
}
void refresh_parking_treeview1(GtkWidget *parking_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Configurer les colonnes (une seule fois)
        setup_parking_treeview2(parking_treeview);
        columns_added = TRUE;
    }

    // Ouvrir le fichier et lire les données
    FILE *file = fopen("parking.txt", "r");
    if (!file) {
        perror("Erreur d'ouverture de parking.txt");
        return;
    }

    // Créer un tableau pour stocker les parkings
    Parking1 parkings[100];  // Vous pouvez ajuster la taille selon vos besoins
    int count = 0;

    char id[10];
    int available_places;

    // Lire les données depuis le fichier
    while (fscanf(file, "%9s %d", id, &available_places) != EOF) {
        // Stocker chaque parking dans le tableau
        strncpy(parkings[count].id, id, sizeof(parkings[count].id) - 1);
        parkings[count].id[sizeof(parkings[count].id) - 1] = '\0';
        parkings[count].available_places = available_places;
        count++;
    }
    fclose(file);

    // Trier les parkings par nombre de places disponibles (tri à bulle)
    bubble_sort_parkings(parkings, count);

    // Créer un GtkListStore pour afficher les données
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);  // ID (string), Places (int)
    
    // Ajouter les parkings triés dans le ListStore
    for (int i = 0; i < count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, parkings[i].id, 1, parkings[i].available_places, -1);
    }

    // Attacher le modèle à la vue
    gtk_tree_view_set_model(GTK_TREE_VIEW(parking_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);  // Libérer la référence à GtkListStore
}
void setup_parking_treeview2(GtkWidget *parking_treeview) {
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    // Colonne pour l'ID du parking
    column = gtk_tree_view_column_new();
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 0);  // Colonne 0 (ID)
    gtk_tree_view_column_set_title(column, "Parking ID");
    gtk_tree_view_append_column(GTK_TREE_VIEW(parking_treeview), column);

    // Colonne pour les places disponibles
    column = gtk_tree_view_column_new();
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 1);  // Colonne 1 (Places)
    gtk_tree_view_column_set_title(column, "Available Places");
    gtk_tree_view_append_column(GTK_TREE_VIEW(parking_treeview), column);
}
int check_parking_id(const char *parking_id, int threshold) {
    ParkingEntry p_list[100];  // Assume max 100 parkings for simplicity
    int count = 0, found = 0;

    // Open file for reading
    FILE *file = fopen("parking.txt", "r");
    if (!file) {
        perror("Failed to open parking.txt");
        return 0;  // Return 0 if the file cannot be opened
    }

    // Read all parkings into memory
    while (fscanf(file, "%49s %d", p_list[count].id, &p_list[count].nbrspaces) == 2) {
        // Check if the parking ID matches the one to be found
        if (strcmp(p_list[count].id, parking_id) == 0) {
            found = 1; // Mark as found
            break; // Exit loop once found
        }
        count++; // Add valid entries to the list
    }
    fclose(file);

    if (!found) {
        printf("Parking ID %s not found.\n", parking_id);
        return 0; // ID not found
    }
    return 1;
    // Check if the number of parking spaces is below the threshold
    if (p_list[count].nbrspaces < threshold) {
        printf("Parking ID %s has fewer than %d available spaces.\n", parking_id, threshold);
        return 0;  // Not enough spaces
    }

    // If found and threshold is met
    printf("Parking ID %s has %d available spaces.\n", parking_id, p_list[count].nbrspaces);
    return 1;  // Success
}


