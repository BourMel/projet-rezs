/**
 *  Utilisé pour le serveur et le client
 */

//message envoyé par le client au server
typedef struct c_a {
  char* action; //put ou get
  char* hash;
} client_action;

//indique une erreur et quitte le programme
void exit_error(char* msg, int sockfd);
