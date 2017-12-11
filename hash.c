#include <stdio.h>
#include <stdlib.h>

/**
 * Déduit un index à partir de l'adresse IP
 */
int hash_value(int key) {
   return key%TABLE_SIZE;
}

/**
 * Affiche les associations IP/FILE de l'ensemble du tableau
 * @TODO corriger le segfault
 */
void display(hash** array) {
  int index = 0;

  while(index < TABLE_SIZE) {
    if(array[index] != NULL) {
        printf("[ %d | %s ] ", array[index]->ip, array[index]->file);
    } else {
      printf("[ - | - ] ");
    }
    index++;
    printf("\n");
  }
}

/**
 *  Place l'adresse du hash à un index calculé
 * @TODO corriger segfault pour certaines valeurs d'insertion
 */
void put(int key, char* value, hash** array) {
  int index;
  hash* new = malloc(sizeof(hash));
  index = hash_value(key);
  new->ip = key;
  new->file = value;

  while(array[index] != NULL && array[index]->ip != -1) {
     index = hash_value(index);
     index++;
  }

  array[index] = new;
}

hash* get(int key, hash** array) {
   int index = hash_value(key);

   while(array[index] != NULL) {
      if(array[index]->ip == key) {
         return array[index];
      }
      index = hash_value(index);
      index++;
   }
   return NULL;
}

// int main(int argc, char * argv[]) {
//
//   hash* array[TABLE_SIZE];
//   put(2, "helloworld", array);
//   put(4, "nothelloworld", array);

  // display(array);

  // printf("%d\n", get(2, array)->ip);
  // printf("%s\n", get(2, array)->file);
  // printf("%d\n", get(4, array)->ip);
  // printf("%s\n", get(4, array)->file);

//   return 0;
// }
