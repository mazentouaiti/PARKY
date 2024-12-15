#ifndef OPERATION_H
#define OPERATION_H
#define MAX_ENTRIES 100
typedef struct {
    int id;
    char name[50];
    char surname[50];
    char email[50];
    char password[50];
    char car_brand[50];
    char car_serial[50];
    float balance;
    char color[50];
    char gender[50];
    char profile_picture[255];
} citizen;
typedef struct {
    char duration[50];
    char date[50];
    float cost;
} parking_data;

int read_citizen_profile(const char *filename, citizen *citizen_data);
int read_citizen_profile_by_id(const char *filename, int id_to_search, citizen *citizen_data);
int search_citizen_by_id(const char *filename, int id_to_search);
int delete_citizen_profile(const char *filename, int id_to_delete);
int search_all_citizens_by_id(const char *filename, int id_to_search, citizen *citizen_data_array, int max_results);
int update_citizen_profile(const char *filename, const citizen *citizen_data);
int read_parking_data(const char *filename, parking_data *data_array, int max_entries);
float calculate_total_cost(const parking_data *data_array, int count);
int add_citizen(const char *filename,citizen *citizen_data);
#endif
