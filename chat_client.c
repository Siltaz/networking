
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "65001"

int main() {

  struct addrinfo hints, *host;
  int sockfd, r;
  char buffer[BUFSIZ], input[BUFSIZ];

  memset_s(&hints, sizeof(hints), 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(0, PORT, &hints, &host) < 0) {
    perror("error");
    exit(1);
  }

  sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
  if (sockfd < 0) {
    perror("error");
    exit(1);
  }

  if (connect(sockfd, host->ai_addr, host->ai_addrlen) < 0) {
    perror("error");
    exit(1);
  }

  while (1) {
    r = recv(sockfd, buffer, BUFSIZ, 0);
    if (r < 1) {
      puts("Connection closed by peer\n");
      break;
    }

    buffer[r] = '\0';
    printf("%s\n", buffer);

    printf("Command: ");
    fgets(input, BUFSIZ, stdin);
    if (strncmp(input, "close", 5) == 0) {
      puts("Closing connection");
      break;
    }

    send(sockfd, input, strlen(input), 0);
  }

  freeaddrinfo(host);
  close(sockfd);
  puts("Disconnected");
  return 0;
}