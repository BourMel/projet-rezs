#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
  int sockfd;
  socklen_t addrlen;
  struct sockaddr_in6 dest;

  // check the number of args on command line
  if (argc != 4) {
    printf("USAGE: %s @dest port_num string\n", argv[0]);
    exit(-1);
  }

  // socket factory
  if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // init remote addr structure and other params
  dest.sin6_family = AF_INET6;
  dest.sin6_port   = htons(atoi(argv[2]));
  addrlen          = sizeof(struct sockaddr_in6);

  // get addr from command line and convert it
  if (inet_pton(AF_INET6, argv[1], &dest.sin6_addr) != 1) {
    perror("inet_pton");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // send string
  if (sendto(sockfd, argv[3], strlen(argv[3]), 0, (struct sockaddr *) &dest, addrlen) == -1) {
    perror("sendto");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // close the socket
  close(sockfd);

  return EXIT_SUCCESS;
}
