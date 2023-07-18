#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "65001"

int main() {
  struct addrinfo hints, *server;
  int sockfd, bytes;
  socklen_t client_size;
  struct sockaddr client;
  char buffer[BUFSIZ], *message = "Hello From UDP Server";

  // Configure server
  memset_s(&hints, sizeof(hints), 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  // Set up server
  if (getaddrinfo(0, PORT, &hints, &server) < 0) {
    perror("Error getting server info");
    exit(1);
  }

  // Create socket
  sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  if (sockfd < 0) {
    perror("Error opening socket");
    exit(1);
  }

  // Bind socket to server address
  if (bind(sockfd, server->ai_addr, server->ai_addrlen) < 0) {
    perror("Error binding socket");
    exit(1);
  }

  puts("UDP server is listening...");

  // Receive data from client
  memset_s(buffer, BUFSIZ, 0, BUFSIZ);
  client_size = sizeof(client);
  bytes = recvfrom(sockfd, &buffer, BUFSIZ, 0, &client, &client_size);
  if (bytes < 0) {
    perror("Error receiving data");
    exit(1);
  }

  buffer[bytes] = '\0';
  printf("CLIENT>> %s\n", buffer);

  // Send response back to client
  if (sendto(sockfd, message, strlen(message), 0, &client, client_size) < 0) {
    perror("Error sending data");
    exit(1);
  }

  // Free allocated memory
  freeaddrinfo(server);

  // Close server socket
  close(sockfd);

  return 0;
}
