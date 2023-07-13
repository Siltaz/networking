#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define PORT "8080"
#define BUFFER_SIZE 1024

int main(){
    struct addrinfo hints, *server;
    int sockfd;
    ssize_t bytes;
    socklen_t client_size;
    struct sockaddr client;
    char buffer[BUFFER_SIZE], *message = "Hello From UDP Server";

    memset_s(&hints, sizeof(hints), 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(0, PORT, &hints, &server) < 0){
        perror("Error getting server info");
        exit(1);
    }

    sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if(sockfd < 0){
        perror("Error opening socket");
        exit(1);
    }

    if(bind(sockfd, server->ai_addr, server->ai_addrlen) < 0){
        perror("Error binding socket");
        exit(1);
    }

    puts("UDP server is listening...");

    memset_s(buffer, BUFFER_SIZE, 0, BUFFER_SIZE);
    client_size = sizeof(client);
    bytes = recvfrom(sockfd, &buffer, BUFFER_SIZE, 0, &client, &client_size);
    if( bytes < 0){
        perror("Error receiving data");
        exit(1);
    }

    printf("Client[%zd]: %s\n", bytes, buffer);

    bytes = sendto(sockfd, message, strlen(message), 0, &client, client_size);
    if(bytes < 0){
        perror("Error sending data");
        exit(1);
    }

    printf("Server[%zd]: %s\n", bytes, message);

    freeaddrinfo(server);
    close(sockfd);

    return 0;
}
