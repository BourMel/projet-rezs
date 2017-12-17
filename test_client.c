#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include "dht.h"

#define CLIENT_TIMEOUT 5
#define CLIENT_WAITTIME 1

// utile si jamais le serveur est hors ligne ou ne répond pas
void * client_timeout(void * arg) {
  int sockfd = * (int *) arg;
  sleep(CLIENT_TIMEOUT);
  printf("Timeout. Le serveur est peut-être indisponible.\n");
  close(sockfd);
  exit(EXIT_SUCCESS);
}

// on attend si jamais d'autres paquets arrivent
void * client_waittime(void * arg) {
  int sockfd = * (int *) arg;
  sleep(CLIENT_WAITTIME);
  close(sockfd);
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  int sockfd;
  socklen_t addrlen;
  struct sockaddr_in6 dest;

  int is_get = 0, hash_len;

  char * server_ip;
  char * server_port;
  char * hash;
  char * put_ip;

  char buf[BUFF_MAX_LENGTH];

  char string_to_send[BUFF_MAX_LENGTH] = { 0 };
  char * tmp;

  // vérification du nombre d'arguments
  if (argc != 5 && argc != 6) {
    fprintf(stderr, "usage: %s IP PORT COMMANDE HASH [IP]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (!strcmp(argv[3], "get")) is_get = 1;
  if (is_get && argc != 5) {
    fprintf(stderr, "usage: %s IP PORT get HASH\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // si c'est autre chose que "get" ou "put"
  if (!is_get && strcmp(argv[3], "put")) {
    fprintf(stderr, "COMMANDE = get | put\n");
    exit(EXIT_FAILURE);
  }

  server_ip = argv[1];
  server_port = argv[2];
  hash = argv[4];
  hash_len = strlen(hash);

  // si c'est un put, on veut qu'une IP soit renseignée
  if (!is_get && argc != 6) {
    fprintf(stderr, "usage: %s IP PORT put HASH IP\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // si c'est un put, on veut une IP (on convertit donc les ndd en IP)
  if (!is_get) {
    put_ip = argv[5];
    char ndd_to_ip[INET6_ADDRSTRLEN] = { 0 };
    if (convert_ndd_to_ip(put_ip, ndd_to_ip)) put_ip = ndd_to_ip;
  }

  if (hash_len < 65 || hash_len > HASH_MAX_LENGTH) {
    fprintf(stderr, "ERR: Le hash doit avoir une longueur comprise entre 65 et %d\n", BUFF_MAX_LENGTH - 2);
    exit(EXIT_FAILURE);
  }

  // on crée le socket
  if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }



  // conversion du nom de domaine en IP
  char ipstr[INET6_ADDRSTRLEN];
  if (convert_ndd_to_ip(server_ip, ipstr)) server_ip = ipstr;

  // on initialise la structure pour la destination
  dest.sin6_family = AF_INET6;
  dest.sin6_port   = htons(atoi(server_port));
  addrlen          = sizeof(struct sockaddr_in6);

  // conversion de l'adresse IP en structure compréhensible pour C
  if (inet_pton(AF_INET6, server_ip, &dest.sin6_addr) != 1) {
    perror("inet_pton");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // on génère la chaîne de caratcères à envoyer
  if (is_get) {
    strcpy(string_to_send, "G");
    tmp = strcat(string_to_send, hash);
    strcpy(string_to_send, tmp);
  } else {
    strcpy(string_to_send, "P");
    tmp = strcat(string_to_send, hash);
    strcpy(string_to_send, tmp);
    tmp = strcat(string_to_send, "\t");
    strcpy(string_to_send, tmp);
    tmp = strcat(string_to_send, put_ip);
    strcpy(string_to_send, tmp);
  }

  // on envoie la chaîne de caractères
  if (sendto(sockfd, string_to_send, strlen(string_to_send), 0, (struct sockaddr *) &dest, addrlen) == -1) {
    perror("sendto");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  if (is_get) {

    pthread_t tid[2];

    shutdown(sockfd, SHUT_WR); // on empêche le client d'écrire dans le socket
    printf("IP disponibles pour le hash %s :\n", hash);

    // un timeout, si jamais le serveur ne répondrait pas
    if ((errno = pthread_create(&tid[0], NULL, client_timeout, &sockfd)) != 0) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }

    while (1) {
      memset(buf, 0, BUFF_MAX_LENGTH);
      if (recvfrom(sockfd, buf, BUFF_MAX_LENGTH, 0, (struct sockaddr *) &dest, &addrlen) == -1) {
        perror("recvfrom");
        close(sockfd);
        exit(EXIT_FAILURE);
      }
      if (*buf == 'S') { // si le serveur nous dit qu'on peut stopper le client
        // on fait attendre un peu histoire d'être sûr de tout avoir reçu
        if ((errno = pthread_create(&tid[1], NULL, client_waittime, &sockfd)) != 0) {
          perror("pthread_create");
          exit(EXIT_FAILURE);
        }
      }
      else printf("%s\n", buf); // on affiche l'IP reçue
    }
  }

  // on ferme le socket
  close(sockfd);

  return EXIT_SUCCESS;
}
