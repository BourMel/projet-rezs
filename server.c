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
//fonctions de la DHT
#include "dht.h"

int main(int argc, char **argv) {
  int sockfd;
  char buf[1024];
  socklen_t addrlen;

  struct sockaddr_in6 my_addr;
  struct sockaddr_in6 client;

  //arguments
  if (argc != 3) {
    printf("Usage: %s domain/IP local_port\n", argv[0]);
    exit(-1);
  }

  if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    exit_error("socket", 0);
  }

  my_addr.sin6_family      = AF_INET6;
  my_addr.sin6_port        = htons(atoi(argv[2]));
  my_addr.sin6_addr        = in6addr_any;
  addrlen                  = sizeof(struct sockaddr_in6);
  memset(buf,'\0',1024);

  //lie l'adresse locale au socket
  if (bind(sockfd, (struct sockaddr *) &my_addr, addrlen) == -1) {
    exit_error("bind", sockfd);
  }

  // reception de la chaine de caracteres
  if (recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *) &client, &addrlen) == -1) {
    exit_error("recvfrom", sockfd);
  }

  printf("%s\n", buf);

  close(sockfd);
  return EXIT_SUCCESS;
}
