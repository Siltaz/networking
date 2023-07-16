#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "8080"
#define BUFFER_SIZE 4096

int main() {
    struct addrinfo hints, *host;
    int sockfd, bytes;
    char buffer[BUFFER_SIZE];

    // Configure server
    memset_s(&hints, sizeof(hints), 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    // Fetch server info
    if (getaddrinfo(0, PORT, &hints, &host) < 0) {
        perror("Error getting server info");
        exit(1);
    }

    // Create socket
    sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    // Connect
    if (connect(sockfd, host->ai_addr, host->ai_addrlen) < 0) {
        perror("Error connecting server");
        exit(1);
    }

    // Sending data
    const char *message = "Hello Server";
    if (send(sockfd, message, strlen(message), 0) == -1) {
        perror("Error sending data to server");
        exit(1);
    }

    // Receive data
    bytes = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytes == -1) {
        perror("Error receiving data from server");
        exit(1);
    }

    // Display received data
    printf("--- Received %d Bytes ---\n%s\n--- Body End ---", bytes, buffer);

    // Close
    freeaddrinfo(host);
    close(sockfd);

    return 0;
}