//taille de la table de hachage
int size = 0;

typedef struct data {
   int key;
   int value;
} hash;

void add(int key, int value, hash* hash_table);
void exit_error(char* msg, int sockfd);
