/**
 *  Fonctions générales utilisées par le serveur et le client
 */

/**
 * Affichage des erreurs et clôture du programme.
 * @param char* msg : message à afficher
 * @param sockfd : identifiant du socket à fermer, 0 s'il n'en existe pas
 */
void exit_error(char* msg, int sockfd) {
  perror(msg);
  if(sockfd == 0) close(sockfd);
  exit(EXIT_FAILURE);
}
