#ifndef PARKING_H
#define PARKING_H
#include <gtk/gtk.h>

typedef struct {
    char park_id[50];
    char status[20];
    char size[20];
    char location[100];
} Parking;
void write_parking_to_file(const char *filename, Parking parking);
void generate_random_parking_id(char *park_id);
//int read_parking_data(const char *filename, Parking parkings[], int max_count);
void write_all_parking_data(const char *filename, Parking parkings[], int count);
int remove_parking(const char *filename, const char *park_id);
int modify_parking(const char *filename, const char *park_id, const char *status, const char *size, const char *location);
void show_tree_view(GtkWidget *button);
int is_parking_id_unique(const char *filename, const char *park_id);
int search_parking_by_id(const char *filename, const char *park_id);

#endif

