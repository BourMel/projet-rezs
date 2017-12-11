#define TABLE_SIZE 20 // taille de la table de hachage
#define BUFF_SIZE 1024 // taille du buffer

typedef struct data {
  int ip;
  char* file;
} hash;

//hash.c
int hash_value(int key);
void display(hash** array);
void put(int key, char* value, hash** array);

//server.c
void exit_error(char* msg, int sockfd);
