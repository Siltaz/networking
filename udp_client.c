#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "65001"

int main()

{
  struct addrinfo hints, *host;
  int sockfd, bytes;
  char buffer[BUFSIZ], *message = "Hi";

  memset_s(&hints, sizeof(hints), 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  if (getaddrinfo(0, PORT, &hints, &host) < 0) {
    perror("Error getting host info");
    exit(1);
  }

  sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
  if (sockfd < 0) {
    perror("Error opening socket");
    exit(1);
  }

  if (sendto(sockfd, message, strlen(message), 0, host->ai_addr,
             host->ai_addrlen) < 0) {
    perror("Error sending data");
    exit(1);
  }

  memset_s(buffer, BUFSIZ, 0, BUFSIZ);
  bytes = recvfrom(sockfd, buffer, BUFSIZ, 0, host->ai_addr, &host->ai_addrlen);
  if (bytes < 0) {
    perror("Error receiving data");
    exit(1);
  }

  buffer[bytes] = '\0';
  printf("SERVER>> %s\n", buffer);

  freeaddrinfo(host);
  close(sockfd);

  return 0;
}
