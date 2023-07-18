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
  const int backlog = 10, clientname_size = 32;
  socklen_t address_len = sizeof(struct sockaddr);
  struct addrinfo hints, *server;
  struct sockaddr address;
  int serverfd, clientfd, max_connect, fd, r, x, done = FALSE;
  char clientname[clientname_size], connection[backlog][clientname_size];
  char buffer[BUFSIZ], send_str[BUFSIZ];
  fd_set main_fd, read_fd;

  memset_s(&hints, sizeof(hints), 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(0, PORT, &hints, &server) < 0) {
    perror("error");
    exit(1);
  }

  serverfd =
      socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  if (serverfd < 0) {
    perror("error");
    exit(1);
  }

  if (bind(serverfd, server->ai_addr, server->ai_addrlen) < 0) {
    perror("error");
    exit(1);
  }

  printf("Server started listening on %s...\n", PORT);
  if (listen(serverfd, backlog) < 0) {
    perror("error");
    exit(1);
  }

  max_connect = backlog;
  FD_ZERO(&main_fd);
  FD_SET(serverfd, &main_fd);

  while (!done) {
    // backup
    read_fd = main_fd;

    if (select(max_connect + 1, &read_fd, NULL, NULL, 0) < 0) {
      perror("error");
      exit(1);
    }

    for (fd = 1; fd <= max_connect; fd++) {
      if (FD_ISSET(fd, &read_fd)) {
        if (fd == serverfd) {
          // accept new connection
          clientfd = accept(fd, &address, &address_len);
          if (clientfd < 0) {
            perror("error");
            exit(1);
          }

          if (getnameinfo(&address, address_len, clientname, clientname_size, 0,
                          0, NI_NUMERICHOST) < 0) {
            perror("error");
            exit(1);
          }

          strcpy(connection[clientfd], clientname);
          printf("New connection from %s\n", connection[clientfd]);

          FD_SET(clientfd, &main_fd);

          // response to new connection
          strcpy(buffer, "SERVER> Welcome ");
          strcat(buffer, connection[clientfd]);
          strcat(buffer, "to the chat server\n");
          strcat(buffer,
                 "SERVER> Type 'close' to disconnect; 'shutdown' to stop\n");
          send(clientfd, buffer, strlen(buffer), 0);

          strcpy(buffer, "SERVER> ");
          strcat(buffer, connection[clientfd]);
          strcat(buffer, " has joined the server\n");

          for (x = serverfd; x <= max_connect; x++) {
            if (FD_ISSET(x, &main_fd)) {
              send(x, buffer, strlen(buffer), 0);
            }
          }

          printf("%s", buffer);
        } else {
          // handle existing connection
          r = recv(fd, buffer, BUFSIZ, 0);
          if (r < 1) {
            FD_CLR(fd, &main_fd);
            close(fd);

            strcpy(buffer, "SERVER> ");
            strcat(buffer, connection[fd]);
            strcat(buffer, " disconnected\n");

            for (x = serverfd; x <= max_connect; x++) {
              if (FD_ISSET(x, &main_fd)) {
                send(x, buffer, strlen(buffer), 0);
              }
            }

            printf("%s", buffer);
          } else {
            buffer[r] = '\0';

            if (strcmp(buffer, "shutdown\n") == 0) {
              done = TRUE;
            } else {
              strcpy(send_str, connection[fd]);
              strcat(send_str, "> ");
              strcat(send_str, buffer);

              for (x = serverfd; x <= max_connect; x++) {
                if (FD_ISSET(x, &main_fd)) {
                  send(x, send_str, strlen(send_str), 0);
                }
              }

              printf("%s", send_str);
            }
          }
        }
      }
    }
  }

  puts("Chat Server shutting down\n");
  freeaddrinfo(server);
  close(serverfd);

  return 0;
}