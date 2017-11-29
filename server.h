#define TABLE_SIZE 128 // taille de la table de hachage
#define BUFF_SIZE 1024 // taille du buffer

typedef struct data {
  int key; // @IP
  char* value; // hash du fichier
} hash;

void add(int key, char* value, hash hash_table[]);
void exit_error(char* msg, int sockfd);
