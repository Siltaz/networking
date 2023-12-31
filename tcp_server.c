#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "8080"

int main() {
  int sockfd, client_sockfd, option, bytes;
  socklen_t client_len;
  struct addrinfo hints, *server_addr;
  struct sockaddr client_addr;
  char buffer[BUFSIZ];

  // Configure server
  memset_s(&hints, sizeof(hints), 0, sizeof(hints));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Set up server
  if (getaddrinfo(0, PORT, &hints, &server_addr) != 0) {
    perror("Error setting up server");
    exit(1);
  }

  // Create socket
  sockfd = socket(server_addr->ai_family, server_addr->ai_socktype,
                  server_addr->ai_protocol);
  if (sockfd < 0) {
    perror("Error opening socket");
    exit(1);
  }

  // Activate dual stack configuration for IPv4 & IPv6
  option = 0;
  if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &option, sizeof(option)) <
      0) {
    perror("Error activating dual stack configuration");
    exit(1);
  }

  // Bind socket to server address
  if (bind(sockfd, server_addr->ai_addr, server_addr->ai_addrlen) < 0) {
    perror("Error binding socket");
    exit(1);
  }

  // Listen for incoming connections
  if (listen(sockfd, 1) < 0) {
    perror("Error listening for connections");
    exit(1);
  }

  printf("Server listening on port %s...\n", PORT);

  // Accept client connection
  client_len = sizeof(client_addr);
  client_sockfd = accept(sockfd, &client_addr, &client_len);
  if (client_sockfd < 0) {
    perror("Error accepting client connection");
    exit(1);
  }

  printf("Client connected\n");

  // Receive data from client
  memset_s(buffer, BUFSIZ, 0, BUFSIZ);
  bytes = recv(client_sockfd, buffer, BUFSIZ - 1, 0);
  if (bytes < 0) {
    perror("Error receiving data from client");
    exit(1);
  }

  buffer[bytes] = '\0';
  printf("CLIENT>> %s\n", buffer);

  // Send response back to client
  const char *response = "HTTP/1.1 200 OK\r\n"
                         "Connection: close\r\n"
                         "Content-Type: text/json\r\n\r\n"
                         "{\"status\":\"ok\"}";
  if (send(client_sockfd, response, strlen(response), 0) < 0) {
    perror("Error sending response to client");
    exit(1);
  }

  // Close client connection
  close(client_sockfd);
  printf("Client disconnected\n");

  // Free allocated memory
  freeaddrinfo(server_addr);

  // Close server socket
  close(sockfd);

  return 0;
}
