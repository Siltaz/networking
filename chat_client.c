
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[]) {

  struct addrinfo hints, *server;
  int sockfd, r, done = FALSE;
  char buffer[BUFSIZ], *host, *port;
  fd_set read_fd;

  if (argc < 2) {
    fprintf(stderr, "Format: client hostname\n");
    exit(1);
  }
  host = argv[1];
  port = argv[2];

  printf("Looking for chat server on %s:%s...", host, port);
  memset_s(&hints, sizeof(hints), 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(0, port, &hints, &server) < 0) {
    perror("error");
    exit(1);
  }
  puts("found");

  sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  if (sockfd < 0) {
    perror("error");
    exit(1);
  }

  if (connect(sockfd, server->ai_addr, server->ai_addrlen) < 0) {
    perror("error");
    exit(1);
  }
  freeaddrinfo(server);

  while (!done) {
    FD_ZERO(&read_fd);
    FD_SET(sockfd, &read_fd);
    FD_SET(0, &read_fd);

    if (select(sockfd + 1, &read_fd, NULL, NULL, 0) < 0) {
      perror("error");
      exit(1);
    }

    if (FD_ISSET(sockfd, &read_fd)) {
      r = recv(sockfd, buffer, BUFSIZ, 0);
      if (r < 1) {
        puts("Connection closed by peer");
        break;
      }
      buffer[r] = '\0';
      printf("%s", buffer);
    }

    if (FD_ISSET(0, &read_fd)) {
      if (fgets(buffer, BUFSIZ, stdin) == NULL) {
        putchar('\n');
      } else if (strcmp(buffer, "close\n") == 0) {
        done = TRUE;
      } else {
        send(sockfd, buffer, strlen(buffer), 0);
      }
    }
  }

  printf("Disconnected\nBye!\n");
  close(sockfd);

  return 0;
}