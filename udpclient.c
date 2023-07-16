#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>

#define PORT "8080"
#define BUFFER_SIZE 1024

int main()  

{
    struct addrinfo hints, *host;
    ssize_t bytes;
    int sockfd;
    char buffer[BUFFER_SIZE], *message = "Hi";

    memset_s(&hints, sizeof(hints), 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(0, PORT, &hints, &host) < 0){
        perror("Error getting host info");
        exit(1);
    }

    sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
    if(sockfd < 0){
        perror("Error opening socket");
        exit(1);
    }

    bytes = sendto(sockfd, message, strlen(message), 0, host->ai_addr, host->ai_addrlen);
    if(bytes < 0){
        perror("Error sending data");
        exit(1);
    }

    printf("Client[%zd]: %s\n", bytes, message);

    bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, host->ai_addr, &host->ai_addrlen);
    if(bytes < 0){
        perror("Error sending data");
        exit(1);
    }

    printf("Server[%zd]: %s\n", bytes, buffer);

    freeaddrinfo(host);
    close(sockfd);

    return 0;
}
