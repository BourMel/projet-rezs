/**
 * Serveur en charge d'une table de hachage
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //close
#include <string.h> //memset

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"

int main(int argc, char **argv) {
  int sockfd;
  char buf[1024];
	socklen_t addrlen;

  struct sockaddr_in6 my_addr;
  struct sockaddr_in6 client;

  hash hash_table[size];

  //arguments
  if(argc != 3) {
    exit_error("usage: ./server IP PORT", 0);
  }

  //création du socket IPv6
  if((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		exit_error("socket", 0);
	}

  //initialisation
  my_addr.sin6_family = AF_INET6;
  my_addr.sin6_port   = htons(atoi(argv[1]));
  my_addr.sin6_addr   = in6addr_any;
  addrlen             = sizeof(struct sockaddr_in6);
	memset(buf,'\0',1024);


  // lien entre adresse locale et socket
	if(bind(sockfd, (struct sockaddr *) &my_addr, addrlen) == -1)
	{
		exit_error("recvfrom", sockfd);
	}

  // reception d'un message
	if(recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *) &client, &addrlen) == -1) {
		exit_error("recvfrom", sockfd);
	}

  //act




  close(sockfd);

  return 0;
}

/**
 *  Ajouter un hash dans la table
 */
void add(int key, int value, hash hash_table[]) {
  // hash *new;
  // int compteur;
  //
  // new = malloc(sizeof(hash)); //FREE
  // if(new == NULL)
  //   exit_error("Impossible d'allouer la mémoire pour l'ajout d'un hash dans la table", 0);
  //
  // new->key = key;
  // new->value = value;
  //
  // //rencontrer une case vide
  // while(hash_table[compteur] != NULL) {
  //   compteur++;
  //   printf("Index du tableau : %d\n", compteur); //debug
  // }
  //
  // hash_table[compteur] = *new;
  // size++;
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
