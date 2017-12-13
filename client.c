#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

//fonctions générales
#include "prog.h"
#include "prog.c" //@TODO ne marchait pas sans...

int main(int argc, char **argv) {
  int sockfd;
  socklen_t addrlen;
  struct sockaddr_in6 dest;

  if(argc != 5 && argc != 6) {
        printf("USAGE: %s @dest port_num commande hash [ip]\n", argv[0]);
        exit(-1);
	}

  if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    exit_error("socket", 0);
  }

  dest.sin6_family = AF_INET6;
  dest.sin6_port   = htons(atoi(argv[2]));
  addrlen          = sizeof(struct sockaddr_in6);

  // convertit l'adresse ip (argument) en adresse IPv6
  if (inet_pton(AF_INET6, argv[1], &dest.sin6_addr) != 1) {
    exit_error("inet_pton", sockfd);
  }

  // envoi de l'information
  if (sendto(sockfd, argv[3], strlen(argv[3]), 0, (struct sockaddr *) &dest, addrlen) == -1) {
    exit_error("sendto", sockfd);
  }

  close(sockfd);

  return EXIT_SUCCESS;
}
