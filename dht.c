#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "dht.h"

// affiche une information
void print_info(char * flag, char * msg) {
  printf(" \e[34m[%s]\e[0m\t %s\n", flag, msg);
}

// affiche une information contenant un string à intégrer
void print_info_str(char * flag, char * msg, char * str) {
  char buffer[BUFF_MAX_LENGTH];
  sprintf(buffer, msg, str);
  print_info(flag, buffer);
}

// affiche une erreur
void print_error(char * msg) {
  printf(" \e[31m[ERR]\e[0m\t %s\n", msg);
}

// affiche un message d'erreur contenant un char à intégrer
void print_error_char(char * msg, char c) {
  char buffer[BUFF_MAX_LENGTH];
  sprintf(buffer, msg, c);
  print_error(buffer);
}

// constructeur pour une liste d'ip
dht_ips new_dht_ips(char * ip) {
  dht_ips ips = malloc(sizeof(struct s_dht_ips));
  if (ips == NULL) {
    fprintf(stderr, "Le malloc n'a pas fonctionné...\n");
    exit(EXIT_FAILURE);
  }
  strcpy(ips->ip, ip);
  ips->next = NULL;
  return ips;
}

// libère la liste chaînée d'IP
void free_dht_ips(dht_ips i) {
  if (i == NULL) return;
  if (i->next != NULL) free_dht_ips(i->next);
  free(i);
}

// constructeur pour une dht
dht new_dht(char * file) {
  int str_len;

  dht d = malloc(sizeof(struct s_dht));
  if (d == NULL) {
    fprintf(stderr, "Le malloc n'a pas fonctionné\n");
    exit(EXIT_FAILURE);
  }
  d->next = NULL;

  // cas de l'initialisation
  if (file == NULL) {
    d->ttl = INT_MAX;
    return d;
  }

  d->ttl = HASH_TTL;

  str_len = strlen(file);
  if (str_len < 65) {
    fprintf(stderr, " [WARN]\t Hash trop petit.\n");
    return NULL;
  } else if (str_len > HASH_MAX_LENGTH) {
    fprintf(stderr, " [WARN]\t Hash trop long.\n");
    return NULL;
  }

  strcpy(d->file, file);
  return d;
}

// permet d'ajouter un IP à une liste d'IPs
void ips_add(dht_ips i, char * ip) {
  int matched_ip = 0;

  if (i == NULL) {
    fprintf(stderr, "La liste d'IPs n'est pas initialisée.\n");
    exit(EXIT_FAILURE);
  }

  // on place le curseur à la fin, sauf si on rencontre déjà l'IP en question
  while (i->next != NULL) {
    if (i->ip != NULL && !strcmp(i->ip, ip)) {
      matched_ip = 1;
      break;
    }
    i = i->next;
  }

  // prise en compte du dernier cas
  if (i->ip != NULL && !strcmp(i->ip, ip)) matched_ip = 1;

  // si on a pas encore vu passer l'IP dans la liste, on l'ajoute
  if (!matched_ip) {
    i->next = new_dht_ips(ip);
  }
}

// permet d'ajouter un couple (hash, ip) à la DHT
void dht_add(dht d, char * hash, char * ip) {
  int matched_hash = 0;

  if (d == NULL) {
    fprintf(stderr, "La DHT n'est pas initialisée.\n");
    exit(EXIT_FAILURE);
  }

  // on place le curseur à la fin, sauf si on rencontre le même hash
  while (d->next != NULL) {
    d = d->next;
    if (d->file != NULL && !strcmp(d->file, hash)) {
      matched_hash = 1;
      break;
    }
  }

  if (matched_hash) { // dans le cas d'un hash déjà connu
    d->ttl = HASH_TTL;
    if (d->ips == NULL) d->ips = new_dht_ips(ip);
    else ips_add(d->ips, ip);
  } else { // dans le cas d'un nouveau hash
    dht n = new_dht(hash);
    if (n != NULL && n->file != NULL) {
      d->next = n;
      d->next->ips = new_dht_ips(ip);
    } else {
      free(n);
    }
  }
}

// fonction qui affiche une liste d'IPs
void dht_print_ips(dht_ips i) {
  dht_ips p = i;
  if (p == NULL) return;
  while (p->next != NULL) {
    print_info_str("I", "    - %s", p->ip);
    p = p->next;
  }
  print_info_str("I", "    - %s", p->ip);
}

// affichage du contenu de la DHT
void dht_print(dht d) {
  if (d == NULL) return;
  if (d->file != NULL && strlen(d->file) > 0) {
    if (d->ips != NULL) {
      char b_info[BUFF_MAX_LENGTH];
      sprintf(b_info, "'%s' [TTL=%d] chez :", d->file, d->ttl);
      print_info("I", b_info);
      dht_print_ips(d->ips);
    } else {
      print_info_str("I", "Personne ne possède le hash '%s'.", d->file);
    }
  }
  dht_print(d->next);
}

// retourne un pointeur vers le hash
dht dht_find_hash(dht d, char * hash) {
  if (d == NULL) return NULL;

  // on parcourt toute la table
  while (d->next != NULL) {
    d = d->next;
    if (d->file != NULL && !strcmp(d->file, hash)) {
      return d;
    }
  }

  return NULL;
}

// supprime un hash de la dht
void dht_delete_hash(dht d, char * hash) {
  if (d == NULL) return;

  dht prev = d, t;

  // on parcourt toute la table
  while (d->next != NULL) {
    prev = d;
    d = d->next;
    if (d->file != NULL && !strcmp(d->file, hash)) {
      t = prev->next; // l'élément courant
      prev->next = d->next;
      free_dht_ips(d->ips);
      free(t);
      return;
    }
  }
}

// convertit un nom de domaine en adresse IP
int convert_ndd_to_ip(char * ndd, char * ipstr) {
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  if (getaddrinfo(ndd, NULL, &hints, &res) == 0) {
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) res->ai_addr;
    inet_ntop(res->ai_family, &(ipv6->sin6_addr), ipstr, INET6_ADDRSTRLEN);
    freeaddrinfo(res);
    return 1;
  }
  return 0;
}

// indique si une chaîne de caractères est une IPv6 valide ou non
int is_valid_ip(char * ip) {
  struct sockaddr_in6 sa;
  return inet_pton(AF_INET6, ip, &(sa.sin6_addr)) != 0;
}
