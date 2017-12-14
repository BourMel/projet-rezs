#include <stdlib.h>
#include <stdio.h>
#include "dht.h"

#define MAX_LENGTH 1024

typedef enum u_data_type {
  DATA_TYPE_GET = 103, // 'g'
  DATA_TYPE_PUT = 112  // 'p'
} data_type;

typedef struct s_data {
  data_type type;
  char * content;
} data;

void data_to_string(data * d, char * out) {
  int i = 1;
  char * p;

  if (d == NULL) {
    fprintf(stderr, "data non initialisé !\n");
    exit(EXIT_FAILURE);
  }

  p = d->content;
  if (p == NULL) {
    fprintf(stderr, "Aucun hash renseigné !\n");
    exit(EXIT_FAILURE);
  }

  out[0] = d->type;
  while (i <= MAX_LENGTH && *p != 0) {
    out[i++] = *p;
    p++;
  }
  if (i <= 65) { // dans le sujet : un hash a une longueur minimale de 65o
    fprintf(stderr, "Hash trop court !\n");
    exit(EXIT_FAILURE);
  }
}

void string_to_data(char * str, data * d) {
  int i = 0;
  char * p = str;
  char content[1 + MAX_LENGTH];
  if (str == NULL || *p == 0) {
    fprintf(stderr, "string non initialisée !\n");
    exit(EXIT_FAILURE);
  }
  if (d == NULL) {
    fprintf(stderr, "data non initialisé !\n");
    exit(EXIT_FAILURE);
  }
  memset(&d->content, 0, 1 + MAX_LENGTH);
  switch (*p++) {
    case DATA_TYPE_PUT:
      d->type = DATA_TYPE_PUT;
      break;
    case DATA_TYPE_GET:
      d->type = DATA_TYPE_GET;
      break;
    default:
      fprintf(stderr, "type non reconnu !\n");
      exit(EXIT_FAILURE);
  }
  while (i <= MAX_LENGTH && *p != 0) content[i++] = *p++;
  content[i] = 0;
  d->content = content;
  if (i < 65) { // dans le sujet : un hash a une longueur minimale de 65o
    fprintf(stderr, "Hash trop court !\n");
    exit(EXIT_FAILURE);
  }
}

int main() {
  // on imagine que c'est un client, qui veut envoyer un hash

  // pour un GET
  char str[1 + MAX_LENGTH];
  memset(str, 0, 1 + MAX_LENGTH);
  data d_get;
  d_get.type = DATA_TYPE_GET;
  d_get.content = "cfKfqjjWrGaIEb8bimkIMfd2TdiCXfpkYkk5HsXbsWAxhfizS64QQlSTpsDUWASfp";
  data_to_string(&d_get, str);
  printf("%s\n", str);

  // pour un PUT
  d_get.type = DATA_TYPE_PUT;
  data_to_string(&d_get, str);
  printf("%s\n", str);

  // on imagine que c'est le serveur qui récupère le hash
  dht d = new_dht(NULL); // quand on commence, la table est vide

  data d_server;
  string_to_data("pcfKfqjjWrGaIEb8bimkIMfd2TdiCXfpkYkk5HsXbsWAxhfizS64QQlSTpsDUWASfp", &d_server);
  printf("CONTENU = %s\n", d_server.content);
  switch (d_server.type) {
    case DATA_TYPE_PUT:
      dht_add(d, d_server.content, "ip");
      break;
    case DATA_TYPE_GET:
      printf("Pas encore implémenté.\n");
      break;
    default:
      fprintf(stderr, "type non reconnu !\n");
      exit(EXIT_FAILURE);
  }

  dht_add(d, "hash2", "ip");
  dht_add(d, "hash", "ip2");
  dht_print(d);

  return EXIT_SUCCESS;
}
