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

int main(int argc, char **argv) {
  int sockfd;
  char buf[1024];
	socklen_t addrlen;

  struct sockaddr_in6 my_addr;
  struct sockaddr_in6 client;

  //arguments
  if(argc != 3) {
    perror("usage: ./server IP PORT");
    exit(EXIT_FAILURE);
  }

  //création du socket IPv6
  if((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
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
		perror("bind");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

  // reception d'un message
	if(recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *) &client, &addrlen) == -1) {
		perror("recvfrom");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

  //act


  close(sockfd);

  return 0;
}
