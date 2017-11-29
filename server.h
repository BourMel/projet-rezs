//taille de la table de hachage
#define TABLE_SIZE 128

typedef struct data {
   int key; //@IP
   char* value; //hash du fichier
} hash;

void add(int key, char* value, hash hash_table[]);
void exit_error(char* msg, int sockfd);
