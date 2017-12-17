#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "dht.h"

// structure pour lister les différents serveurs
typedef struct s_servers {
  int fd;
  struct s_servers * next;
} * servers;

// structure à envoyer pour gérer les keep-alive
struct s_ka {
  int sockfd;
  struct sockaddr_in6 * client;
  socklen_t addrlen;
};

// structure à passer au manager de serveurs pour avoir de quoi travailler :p
struct s_datas_servers {
  dht d;
  servers s;
  char * ip;
  char * port;
};

// on initialise une liste pour stocker les serveurs
servers new_servers() {
  servers s = malloc(sizeof(struct s_servers));
  if (s == NULL) {
    fprintf(stderr, "Le malloc a échoué !\n");
    exit(EXIT_FAILURE);
  }
  s->fd = -1;
  s->next = NULL;
  return s;
}

void print_servers(servers s) {
  servers p = s;
  // printf("Liste des serveurs connectés :\n");
  while (p != NULL) {
    if (p->fd != -1) printf(" - %d\n", p->fd);
    p = p->next;
  }
}

void add_server(servers s, int fd) {
  servers p = s;
  int seen = 0;
  while (p != NULL && p->fd != fd) {
    if (p->fd == fd) seen = 1;
    if (p->next != NULL) p = p->next;
    else break;
  }
  if (!seen) {
    while (p->next != NULL) p = p->next;
    p->next = new_servers();
    p->next->fd = fd;
  } else if (seen) {
    // @TODO: reset ici le temps
  }
}


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
int reply_str(int sockfd, char * str, struct sockaddr_in6 * client, socklen_t addrlen) {
  if (sendto(sockfd, str, strlen(str), 0, (struct sockaddr *) client, addrlen) == -1) {
    print_error("Erreur lors de la réponse; le socket est peut-être fermé.");
    close(sockfd);
    return 0;
  }
  return 1;
}

// gestion des TTL
void * manage_ttl(void * arg) {
  dht d = (dht) arg, p;
  print_info("I", "TTL_manager OK.");

  while (1) {
    sleep(1);
    p = d;
    while (p != NULL) {
      // cas des hash immortels (le premier, qui est vide,  par exemple)
      if (p->ttl > HASH_TTL) {
        p = p->next;
        continue;
      }
      // on supprime si le hash est obsolète
      if (p->file != NULL && --p->ttl <= 0) {
        dht_delete_hash(d, p->file);
      }
      p = p->next;
    }
  }

  return NULL;
}

// permet de gérer les autres serveurs
void * manage_servers(void * arg) {
  (void) arg; // pour pas que le compilo ne crie
  // struct s_datas_servers * ds = (struct s_datas_servers *) arg;
  // dht p = ds->d;
  // servers s = ds->s;

  while (1) {
    sleep(2);
    // dht_print(p);
    // print_servers(s);
    // print_info("I", "Servers_manager working...");
    // en fait non, pas trop...
  }

  return NULL;
}

// permet de gérer le keep-alive
void * manage_ka(void * arg) {

  struct s_ka * ka = (struct s_ka *) arg;

  sleep(5);
  while (reply_str(ka->sockfd, "A", ka->client, ka->addrlen)) {
    sleep(5); // on envoie un "A" toutes les 5 sec pour dire qu'on est toujours là
  }

  return NULL;
}

int main(int argc, char **argv) {
  int sockfd, c_sockfd;
  char buf[BUFF_MAX_LENGTH];
  char ipstr[INET6_ADDRSTRLEN];
  char c_ipstr[INET6_ADDRSTRLEN];
  socklen_t addrlen;

  struct sockaddr_in6 my_addr, c_my_addr, client;

  char *hash, *p_hash, *put_ip_cursor, *server_ip, *c_server_ip, *tmp;
  char put_ip[INET6_ADDRSTRLEN];

  int server_running = 1;

  pthread_t tid[3];

  struct s_ka * ka;


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

  // si le serveur doit se connecter à un autre serveur
  if (argc == 5) {
    // création du socket
    if ((c_sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      perror("socket");
      close(sockfd);
      exit(EXIT_FAILURE);
    }

    // initialisation de la structure et d'autres paramètres
    c_server_ip = argv[3];
    if (convert_ndd_to_ip(c_server_ip, c_ipstr)) c_server_ip = c_ipstr;
    c_my_addr.sin6_family      = AF_INET6;
    c_my_addr.sin6_port        = htons(atoi(argv[4]));
    addrlen                    = sizeof(struct sockaddr_in6);

    if (inet_pton(AF_INET6, c_server_ip, &c_my_addr.sin6_addr) != 1) {
      perror("inet_pton");
      close(sockfd);
      exit(EXIT_FAILURE);
    }

    // on bind la structure avec le socket
    if (connect(c_sockfd, (struct sockaddr *) &c_my_addr, addrlen) == -1) {
      perror("connect");
      close(c_sockfd);
      close(sockfd);
      exit(EXIT_FAILURE);
    }

    reply_str(c_sockfd, "A", &c_my_addr, addrlen);

    ka = malloc(sizeof(struct s_ka));
    ka->sockfd = c_sockfd;
    ka->client = &c_my_addr;
    ka->addrlen = addrlen;

    // on lance un thread qui va que s'occuper du keep-alive
    if ((errno = pthread_create(&tid[2], NULL, manage_ka, ka)) != 0) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }

  dht d = new_dht(NULL); // quand on commence, la table est vide
  servers s = new_servers();
  struct s_datas_servers ds = { d, s, server_ip, argv[2] };

  // on lance un thread qui va que s'occuper des TTL
  if ((errno = pthread_create(&tid[0], NULL, manage_ttl, d)) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  // on lance un thread qui va que s'occuper des autres serveurs
  if ((errno = pthread_create(&tid[1], NULL, manage_servers, &ds)) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

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
          print_info_str("I", " \\__IP ajoutée = %s", put_ip);
          dht_print(d);
          if (argc == 5) { // si on doit aussi envoyer à un autre serveur
            char str_to_put[BUFF_MAX_LENGTH];
            memset(str_to_put, 0, BUFF_MAX_LENGTH);
            strcpy(str_to_put, "P");
            tmp = strcat(str_to_put, hash);
            strcpy(str_to_put, tmp);
            tmp = strcat(str_to_put, "\t");
            strcpy(str_to_put, tmp);
            tmp = strcat(str_to_put, put_ip);
            strcpy(str_to_put, tmp);
            reply_str(ka->sockfd, str_to_put, ka->client, ka->addrlen);
          }
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
      case 'A': // un serveur souhaite s'ajouter
        add_server(s, sockfd);
        break;
      default:
        print_error_char("Action '%c' non reconnue. On ignore.", *buf);
    }
  }

  // on ferme le socket
  close(sockfd);

  return EXIT_SUCCESS;
}
