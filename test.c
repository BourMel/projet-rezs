#include <stdlib.h>
#include <stdio.h>
#include "dht.h"

int main() {

  dht d = new_dht(NULL); // quand on commence, la table est vide
  dht_add(d, "hash", "ip");
  dht_add(d, "hash2", "ip");
  dht_add(d, "hash", "ip2");
  dht_print(d);

  return EXIT_SUCCESS;
}
