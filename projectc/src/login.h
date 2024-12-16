typedef struct 
{ char name[30];
  char surname [30];
  char login[30];
  char pass[30];
  char mail[30];
  char sex[20];
}utilisateur;

void create_account(const char *filename, utilisateur u);
void sex(int valeur, char *msg);
int log_in(const char *filename, const char *login, const char *pass);

