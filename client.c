#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// l'envoi n'est pas encore détecté par le serveur

int main(int argc, char **argv) {
  int sockfd;
  socklen_t addrlen;
  struct sockaddr_in6 dest;

  // arguments
  if (argc != 5 && argc != 6) {
    printf("USAGE: %s @dest port_num commande hash [ip]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

	if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	dest.sin6_family = AF_INET6;
	dest.sin6_port   = htons(atoi(argv[2]));
	addrlen          = sizeof(struct sockaddr_in6);

	// récupère l'adresse en argument
	if (inet_pton(AF_INET6, argv[1], &dest.sin6_addr) != 1) {
		perror("inet_pton");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	// envoi de la chaine de caractères
	if (sendto(sockfd, argv[3], strlen(argv[3]), 0,
    (struct sockaddr *) &dest, addrlen) == -1) {
		perror("sendto");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	close(sockfd);

	return 0;
}
