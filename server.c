#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "dht.h"

// permet d'enlever la première occurence de c et tout ce qui suit d'une string
void str_remove_after(char * str, char c) {
  int see_char = 0;
  while (*str != 0) {
    if (c == '\n') {
      if (*str == '\n' || *str == '\r') {
        see_char = 1;
      }
    } else {
      if (*str == c) see_char = 1;
    }
    if (see_char) *str = 0;
    str++;
  }
}

// Envoie une chaîne de caractères
void reply_str(int sockfd, char * str, struct sockaddr_in6 * client, socklen_t addrlen) {
  if (sendto(sockfd, str, strlen(str), 0, (struct sockaddr *) client, addrlen) == -1) {
    print_error("Erreur lors de la réponse; le socket est peut-être fermé.");
    close(sockfd);
  }
}

int main(int argc, char **argv) {
  int sockfd;
  char buf[BUFF_MAX_LENGTH];
  char ipstr[INET6_ADDRSTRLEN];
  socklen_t addrlen;

  struct sockaddr_in6 my_addr, client;

  char *hash, *p_hash, *put_ip_cursor, *server_ip;
  char put_ip[INET6_ADDRSTRLEN];

  int server_running = 1;

  // vérifie le nombre d'arguments
  if (argc != 3 && argc != 5) {
    printf("Usage: %s IP PORT [distIPserver distPORTserver]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // création du socket
  if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // initialisation de la structure et d'autres paramètres
  server_ip = argv[1];
  if (convert_ndd_to_ip(server_ip, ipstr)) server_ip = ipstr;
  my_addr.sin6_family      = AF_INET6;
  my_addr.sin6_port        = htons(atoi(argv[2]));
  addrlen                  = sizeof(struct sockaddr_in6);

  if (inet_pton(AF_INET6, server_ip, &my_addr.sin6_addr) != 1) {
    perror("inet_pton");
    exit(EXIT_FAILURE);
  }

  // on bind la structure avec le socket
  if (bind(sockfd, (struct sockaddr *) &my_addr, addrlen) == -1) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  dht d = new_dht(NULL); // quand on commence, la table est vide

  while (server_running) {
    memset(buf, 0, BUFF_MAX_LENGTH);

    // réception de la chaine de caracteres
    if (recvfrom(sockfd, buf, BUFF_MAX_LENGTH, 0, (struct sockaddr *) &client, &addrlen) == -1) {
      perror("recvfrom");
      close(sockfd);
      exit(EXIT_FAILURE);
    }

    hash = buf + 1;
    p_hash = hash;

    // on enlève les sauts de lignes (dans le cas des tests avec `nc`)
    str_remove_after(p_hash, '\n');

    // si jamais on a une IP passée en arg
    put_ip_cursor = hash;
    while (put_ip_cursor != 0) {
      if (*put_ip_cursor == '\t') break;
      put_ip_cursor++;
    }
    if (*put_ip_cursor != 0) {
      memset(put_ip, 0, INET6_ADDRSTRLEN);
      strcpy(put_ip, ++put_ip_cursor);
      put_ip_cursor = hash;
    }
    str_remove_after(put_ip_cursor, '\t');

    switch (*buf) {
      case 'P': // pour dire que l'IP possède tel hash
        print_info("PUT", hash);
        if (strlen(put_ip)) {
          dht_add(d, hash, put_ip);
          print_info_str("I", "Source = %s", put_ip);
          dht_print(d);
        } else {
          print_error("Pas d'IP associé à ce hash. Abandon.");
        }
        break;
      case 'G': // pour récupérer les IP qui possèdent ce hash
        print_info("GET", hash);
        dht dht_hash = dht_find_hash(d, hash);
        if (dht_hash == NULL) reply_str(sockfd, "S", &client, addrlen); // stop
        else if (dht_hash->ips == NULL) reply_str(sockfd, "S", &client, addrlen);
        else {
          dht_ips p = dht_hash->ips;
          while (p->next != NULL) {
            if (p->ip != NULL) reply_str(sockfd, p->ip, &client, addrlen);
            p = p->next;
          }
          if (p->ip != NULL) reply_str(sockfd, p->ip, &client, addrlen);
          reply_str(sockfd, "S", &client, addrlen);
        }
        break;
      case 'V': // pour voir le statut de la DHT sur le serveur
        dht_print(d);
        break;
      case 'S': // on stoppe le serveur
        print_info("QUIT", "Le serveur s'arrête.");
        server_running = 0;
        break;
      case '\n': // quand on joue avec `nc` ..
        break;
      default:
        print_error_char("Action '%c' non reconnue. On ignore.", *buf);
    }
  }

  // on ferme le socket
  close(sockfd);

  return 0;
}
