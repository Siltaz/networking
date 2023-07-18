#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_select.h>
#include <sys/_types/_socklen_t.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "65001"
#define TRUE 1
#define FALSE 0

int main() {
  const int backlog = 10, hostname_size = 32;
  const char *welcome_msg = "Type 'close' to disconnect; 'shutdown' to stop\n";
  socklen_t client_len = sizeof(struct sockaddr);
  struct addrinfo hints, *server;
  struct sockaddr client;
  int sockfd, clientfd, max_connect, fd, r, done = FALSE;
  char hostname[hostname_size], connection[backlog][hostname_size];
  char buffer[BUFSIZ];
  fd_set main_fd, read_fd;

  memset_s(&hints, sizeof(hints), 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(0, PORT, &hints, &server) < 0) {
    perror("error");
    exit(1);
  }

  sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  if (sockfd < 0) {
    perror("error");
    exit(1);
  }

  if (bind(sockfd, server->ai_addr, server->ai_addrlen) < 0) {
    perror("error");
    exit(1);
  }

  printf("Server started listening on %s...\n", PORT);
  if (listen(sockfd, backlog) < 0) {
    perror("error");
    exit(1);
  }

  max_connect = backlog;
  FD_ZERO(&main_fd);
  FD_SET(sockfd, &main_fd);

  while (!done) {
    // backup
    read_fd = main_fd;

    if (select(max_connect + 1, &read_fd, NULL, NULL, 0) < 0) {
      perror("error");
      exit(1);
    }

    for (fd = 1; fd <= max_connect; fd++) {
      if (FD_ISSET(fd, &read_fd)) {
        if (fd == sockfd) {
          // accept new connection
          clientfd = accept(fd, &client, &client_len);
          if (clientfd < 0) {
            perror("error");
            exit(1);
          }

          if (getnameinfo(&client, client_len, hostname, hostname_size, 0, 0,
                          NI_NUMERICHOST) < 0) {
            perror("error");
            exit(1);
          }

          strcpy(connection[clientfd], hostname);
          printf("New connection from %s\n", connection[clientfd]);

          FD_SET(clientfd, &main_fd);

          // response to new connection
          strcpy(buffer, "Hello to ");
          strcpy(buffer, connection[clientfd]);
          strcpy(buffer, "!\n");
          strcpy(buffer, welcome_msg);
          send(clientfd, buffer, strlen(buffer), 0);
        } else {
          // handle existing connection
          r = recv(fd, buffer, BUFSIZ, 0);
          if (r < 1) {
            FD_CLR(fd, &main_fd);
            close(fd);
            printf("%s closed\n", connection[fd]);
          } else {
            buffer[r] = '\0';
            if (strcmp(buffer, "shutdown\n") == 0) {
              done = TRUE;
            } else {
              send(fd, buffer, strlen(buffer), 0);
            }
          }
        }
      }
    }
  }

  puts("Chat Server shutting down\n");
  freeaddrinfo(server);
  close(sockfd);

  return 0;
}