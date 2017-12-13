#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
    if (d->file != NULL && !strcmp(d->file, hash)) {
      matched_hash = 1;
      break;
    }
    d = d->next;
  }

  if (matched_hash) { // dans le cas d'un hash déjà connu
    if (d->ips == NULL) d->ips = new_dht_ips(ip);
    else ips_add(d->ips, ip);
  } else { // dans le cas d'un nouveau hash
    d->next = new_dht(hash);
    d->next->ips = new_dht_ips(ip);
  }
}


// fonction qui affiche une liste d'IPs
void dht_print_ips(dht_ips i) {
  if (i == NULL) return;
  while (i->next != NULL) {
    printf("    - %s\n", i->ip);
    i = i->next;
  }
  printf("    - %s\n", i->ip);
}

// fonction auxilière pour afficher le contenu d'une DHT
void dht_print_aux(dht d) {
  if (d == NULL) return;
  if (d->ips != NULL) {
    printf("  Les IP possédant le hash '%s' sont :\n", d->file);
    dht_print_ips(d->ips);
    printf("\n");
  } else {
    printf("  Personne ne possède le hash '%s'...\n", d->file);
  }
  dht_print_aux(d->next);
}

// affichage du contenu de la DHT
void dht_print(dht d) {
  if (d == NULL) {
    fprintf(stderr, "La DHT n'est pas initialisée.\n");
    exit(EXIT_FAILURE);
  }
  printf("CONTENU DE LA DHT :\n\n");
  if (d->file == NULL) dht_print_aux(d->next); // si c'est le premier
  else dht_print_aux(d);
}


int main() {

  dht d = new_dht(NULL); // quand on commence, la table est vide
  dht_add(d, "hash", "ip");
  dht_add(d, "hash2", "ip");
  dht_add(d, "hash", "ip2");
  dht_print(d);

  return EXIT_SUCCESS;
}
