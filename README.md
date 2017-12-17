Table de hachage distribuée
===========================

> Projet de réseaux et protocoles pour le S5

## Documentation

A l'aide du terminal, se placer dans le répertoire du projet et entrer
`man ./server.1` ou `man ./client.1` en fonction de l'information souhaitée.


## Format des messages échangés

Les informations échangées sont sous forme de chaîne de caractères.

La première lettre indique le type d'action à effectuer.

  - `P` comme **PUT** permet d'envoyer un couple (hash, ip) à un serveur. La
    chaîne commence par `P`, directement suivie par le hash, ensuite une
    tabulation (moins de risque accidentel d'insertion par rapport aux espaces),
    puis enfin l'adresse IP.

  - `G` comme **GET** permet de demander à un serveur la liste des IP qui
    possèdent le hash. La chaîne commence par `G` et est directement suivie par
    le hash. Le serveur répondra directement en clair au client IP par IP ; le
    fait de faire cela sous forme de plusieurs messages permet en cas de perte
    d'un paquet que le client puisse tout de même recevoir un certain nombre
    d'adresses IP. Si jamais le client ne reçoit pas de réponse au bout de 5
    secondes (timeout par défaut) il quitte pour ne pas attendre
    indéfiniment. Une fois que le serveur a envoyé un message de type `S` (voir
    le point suivant), le client attend une seconde histoire d'être certain d'avoir
    récupéré tous les paquets, puis se termine.

  - `S` comme **STOP**, permet, s'il est envoyé au serveur, de l'arrêter
    directement (pratique pour les tests avec la commande `nc`). S'il est envoyé
    au client par le serveur, c'est que le serveur a terminé d'envoyer des
    données au client, et ce dernier peut donc librement fermer le socket
    utilisé.

  - `V` comme **VIEW**, pratique pour les tests à partir de `nc`. Quand on
    envoie ce message, le serveur affiche simplement le contenu de ses tables
    avec les IP correspondantes; très pratique pour voir si tout se passe comme
    prévu.

  - `A` comme **ADD**, permet d'ajouter un serveur.


## Lancement des tests

Lancez simplement la commande `make tests`  pour lancer les tests.

Il est également possible de tester rapidement en utilisant la commande
`nc -u ::1 1234` pour envoyer rapidement des messages à la main sur le serveur
(remplacer `::1` par l'IP du serveur et `1234` par le port).
