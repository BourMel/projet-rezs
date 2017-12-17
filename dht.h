#ifndef __HASH_H
#define __HASH_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "dht.h"

#define BUFF_MAX_LENGTH 1024
#define HASH_MAX_LENGTH BUFF_MAX_LENGTH-INET6_ADDRSTRLEN-3

// Liste chaînée d'IP
typedef struct s_dht_ips {
  char ip[INET6_ADDRSTRLEN];
  struct s_dht_ips * next;
} * dht_ips;

// Contenu de la DHT
typedef struct s_dht {
  dht_ips ips;
  char file[HASH_MAX_LENGTH]; // hash
  struct s_dht * next;
} * dht;


// affiche une information
void print_info(char * flag, char * msg);

// affiche une information contenant un string à intégrer
void print_info_str(char * flag, char * msg, char * str);

// affiche une erreur
void print_error(char * msg);

// affiche un message d'erreur contenant un char à intégrer
void print_error_char(char * msg, char c);

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

// affichage du contenu de la DHT
void dht_print(dht d);

// retourne un pointeur vers le hash
dht dht_find_hash(dht d, char * hash);

// convertit un nom de domaine en adresse IP
int convert_ndd_to_ip(char * ndd, char * ipstr);

#endif
