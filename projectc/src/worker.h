typedef struct
   {char workerid[20];
    char workername[50];
    char workerparkid[50];
   }worker;
int assign_worker_to_parking(const char *filename, const char *worker_id, const char *parking_id);
int unassign_worker(const char *filename, const char *worker_id);
void show_worker_tree_view(GtkWidget *button);
int is_park_id_exist(const char *filename, const char *park_id);
void parkingremoved(const char *filename, const char *park_id);
int searchworker_by_id(const char *filename, const char *worker_id);
