#ifndef __HASH_H
#define __HASH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dht.h"

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
dht_ips new_dht_ips(char * ip);

// constructeur pour une dht
dht new_dht(char * file);

// permet d'ajouter un IP à une liste d'IPs
void ips_add(dht_ips i, char * ip);

// permet d'ajouter un couple (hash, ip) à la DHT
void dht_add(dht d, char * hash, char * ip);

// fonction qui affiche une liste d'IPs
void dht_print_ips(dht_ips i);

// fonction auxilière pour afficher le contenu d'une DHT
void dht_print_aux(dht d);

// affichage du contenu de la DHT
void dht_print(dht d);

#endif
