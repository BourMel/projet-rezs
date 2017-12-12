#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

// Liste chaînée d'IP
typedef struct s_dht_ips {
  char * ip; // @TODO: changer le type
  struct s_dht_ips * next;
} * dht_ips;

// Contenu de la DHT
typedef struct s_dht {
  dht_ips ips;
  char * file; // hash
  struct s_dht * next;
} * dht;

// constructeur pour une liste d'ip
dht_ips new_dht_ips(char * ip) {
  dht_ips ips = malloc(sizeof(struct s_dht_ips));
  if (ips == NULL) {
    fprintf(stderr, "Le malloc n'a pas fonctionné...\n");
    exit(EXIT_FAILURE);
  }
  ips->ip = ip;
  ips->next = NULL;
  return ips;
}

// constructeur pour une dht
dht new_dht(char * file) {
  dht d = malloc(sizeof(struct s_dht));
  if (d == NULL) {
    fprintf(stderr, "Le malloc n'a pas fonctionné\n");
    exit(EXIT_FAILURE);
  }
  d->file = file;
  d->next = NULL;
  return d;
}

void dht_add(dht d, char * hash, char * ip) {
  if (d == NULL) {
    fprintf(stderr, "La DHT n'est pas initialisée.\n");
    exit(EXIT_FAILURE);
  }
  while (d->next != NULL) d = d->next; // @TODO: ajouter strcmp d->file et hash
  d->next = new_dht(hash);
  d->next->ips = new_dht_ips(ip);
}


// fonction qui affiche une liste d'IPs
void dht_print_ips(dht_ips i) {
  if (i == NULL) return;
  while (i->next != NULL) {
    printf("\t - %s\n", i->ip);
    i = i->next;
  }
  printf("\t - %s\n", i->ip);
}

// fonction auxilière pour afficher le contenu d'une DHT
void dht_print_aux(dht d) {
  if (d == NULL) return;
  if (d->ips != NULL) {
    printf("Les IP possédant le hash '%s' sont :\n", d->file);
    dht_print_ips(d->ips);
    printf("\n\n\n");
  } else {
    printf("Personne ne possède le hash '%s'...\n\n", d->file);
  }
  dht_print_aux(d->next);
}

// affichage du contenu de la DHT
void dht_print(dht d) {
  if (d == NULL) {
    fprintf(stderr, "La DHT n'est pas initialisée.\n");
    exit(EXIT_FAILURE);
  }
  printf("La DHT contient actuellement les hash suivants :\n");
  if (d->file == NULL) dht_print_aux(d->next); // si c'est le premier
  else dht_print_aux(d);
}


int main() {

  dht d = new_dht(NULL); // quand on commence, la table est vide
  dht_add(d, "hash", "ip");
  dht_print(d);

  return EXIT_SUCCESS;
}
