#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#define CRLF    "\r\n"
#define PORT    1977
#define MAX_CLIENTS   100

#define BUF_SIZE  1024

typedef struct {
 struct sockaddr_in sin;
 char name[BUF_SIZE];
} Client;


void write_client(SOCKET sock, struct sockaddr_in *sin, const char *buffer) {
  if (sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *) sin, sizeof *sin) < 0) {
    perror("send()");
    exit(errno);
  }
}


int check_if_client_exists(Client *clients, struct sockaddr_in *csin, int actual) {
  int i = 0;
  for (i = 0; i < actual; i++) {
    if (clients[i].sin.sin_addr.s_addr == csin->sin_addr.s_addr
      && clients[i].sin.sin_port == csin->sin_port) {
      return 1;
    }
  }
  return 0;
}

Client* get_client(Client *clients, struct sockaddr_in *csin, int actual) {
  int i = 0;
  for (i = 0; i < actual; i++) {
    if (clients[i].sin.sin_addr.s_addr == csin->sin_addr.s_addr
      && clients[i].sin.sin_port == csin->sin_port) {
      return &clients[i];
    }
  }
  return NULL;
}

void remove_client(Client *clients, int to_remove, int *actual) {
  /* we remove the client in the array */
  memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove) * sizeof(Client));
  /* number client - 1 */
  (*actual)--;
}

void send_message_to_all_clients(int sock, Client *clients, Client *sender, int actual, const char *buffer, char from_server) {
  int i = 0;
  char message[BUF_SIZE];
  message[0] = 0;
  for (i = 0; i < actual; i++) {
    /* we don't send message to the sender */
    if (sender != &clients[i]) {
      if (from_server == 0) {
        strncpy(message, sender->name, BUF_SIZE - 1);
        strncat(message, " : ", sizeof message - strlen(message) - 1);
      }
      strncat(message, buffer, sizeof message - strlen(message) - 1);
      write_client(sock, &clients[i].sin, message);
    }
  }
}

int init_connection(void) {
  SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in sin = { 0 };

  if (sock == INVALID_SOCKET) {
    perror("socket()");
    exit(errno);
  }

  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(PORT);
  sin.sin_family = AF_INET;

  if (bind(sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR) {
    perror("bind()");
    exit(errno);
  }

  return sock;
}

int read_client(SOCKET sock, struct sockaddr_in *sin, char *buffer) {
  int n = 0;
  socklen_t sinsize = sizeof *sin;

  if ((n = recvfrom(sock, buffer, BUF_SIZE - 1, 0, (SOCKADDR *) sin, &sinsize)) < 0) {
    perror("recvfrom()");
    exit(errno);
  }

  buffer[n] = 0;

  return n;
}


void app(void) {
  SOCKET sock = init_connection();
  char buffer[BUF_SIZE];
  /* the index for the array */
  int actual = 0;
  int max = sock;
  /* an array for all clients */
  Client clients[MAX_CLIENTS];

  fd_set rdfs;

  while (1) {
    FD_ZERO(&rdfs);

    /* add STDIN_FILENO */
    FD_SET(STDIN_FILENO, &rdfs);

    /* add the connection socket */
    FD_SET(sock, &rdfs);

    if (select(max + 1, &rdfs, NULL, NULL, NULL) == -1) {
      perror("select()");
      exit(errno);
    }

    /* something from standard input : i.e keyboard */
    if (FD_ISSET(STDIN_FILENO, &rdfs)) {
      /* stop process when type on keyboard */
      break;
    } else if (FD_ISSET(sock, &rdfs)) {
      /* new client */
      struct sockaddr_in csin = { 0 };

      /* a client is talking */
      read_client(sock, &csin, buffer);

      if (check_if_client_exists(clients, &csin, actual) == 0) {
        if (actual != MAX_CLIENTS) {
          Client c = { csin, "test" };
          strncpy(c.name, buffer, BUF_SIZE - 1);
          clients[actual] = c;
          actual++;
        }
      } else {
        Client *client = get_client(clients, &csin, actual);
        if (client == NULL) continue;
        send_message_to_all_clients(sock, clients, client, actual, buffer, 0);
      }
    }
  }
  close(sock);
}


int main() {
  app();
  return EXIT_SUCCESS;
}
