/**
 * Serveur en charge d'une table de hachage
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // close
#include <string.h> // memset, strstr

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"

int main(int argc, char * argv[]) {
  int sockfd;
  char buf[BUFF_SIZE];
  socklen_t addrlen;
  char * action;

  struct sockaddr_in6 my_addr;
  struct sockaddr_in6 client;

  hash hash_table[TABLE_SIZE];

  // arguments
  if (argc != 3) {
    exit_error("usage: ./server IP PORT", 0);
  }

  // création du socket IPv6
  if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    exit_error("socket", 0);
  }

  //initialisation
  my_addr.sin6_family = AF_INET6;
  my_addr.sin6_port   = htons(atoi(argv[1]));
  my_addr.sin6_addr   = in6addr_any;
  addrlen             = sizeof(struct sockaddr_in6);
  memset(buf, '\0', BUFF_SIZE);


  // lien entre adresse locale et socket
  if (bind(sockfd, (struct sockaddr *) &my_addr, addrlen) == -1) {
    exit_error("recvfrom", sockfd);
  }

  // réception d'un message
  if (recvfrom(sockfd, buf, BUFF_SIZE, 0, (struct sockaddr *) &client, &addrlen) == -1) {
    exit_error("recvfrom", sockfd);
  }

  // décomposer le message envoyé par le client (GET/PUT + hash)
  // savoir si le buffer contient "GET"
  if (strstr(buf, "GET") != NULL) { // remplacer par une découpe de la chaîne de caractères
    action = "GET";

  } else if (strstr(buf, "PUT") != NULL) {
    action = "PUT";
  }

  if (strcmp(action, "GET") == 0) {
    // renvoyer les IP en possession de ce hash

  } else if (strcmp(action, "PUT")) {
    // le client indique qu'il a le fichier
    add(42, buf, hash_table); // remplacer buf par le hash récupéré
    // adresse du client : client.sin_addr ?

  } else {
    printf("Le message reçu n'a pas été traité\n");
  }

  close(sockfd);

  return 0;
}

/**
 *  Ajouter un hash dans la table
 */
void add(int key, char* value, hash hash_table[]) {
  int index = (key % TABLE_SIZE);

  printf("%s\n", hash_table[index].value);

  // while (hash_table[index].value != 0 && hash_table[index].value != key) {
    index = (index + 1) % TABLE_SIZE;
  // }

  printf("Index choisi : %d", index);

  hash_table[index].key = key;
  hash_table[index].value = value;
}

/**
 * Affichage des erreurs et clôture du programme.
 * S'il n'y a pas de socket créé, indiquer sockfd = 0
 */
void exit_error(char* msg, int sockfd) {
  perror(msg);
  if(sockfd == 0) close(sockfd);
  exit(EXIT_FAILURE);
}
