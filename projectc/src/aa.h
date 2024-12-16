#ifndef PROGRAM_H
#define PROGRAM_H


// Structure pour représenter une entrée de parking
typedef struct {
    char id[50];
    char entered_date[50];
    int nbrspaces;
    char matricule[50];
    char phone[50];
} ParkingEntry;
typedef struct {
    char id[10];            // ID du parking
    int available_places;   // Nombre de places disponibles dans le parking
} Parking1;

void edit_parking(const char *filename, const char *parking_id, ParkingEntry *updated_parking);
int validateDate(const char *date);
int delete_parking(const char *filename, const char *parking_id);

#endif
