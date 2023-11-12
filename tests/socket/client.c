// Client side C/C++ program to demostrate Socket Programming
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int main(int argc, char* argv[])
{
    printf("argc: %d, argv[0]: %s\n", argc, argv[0]);
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
if(argc>1) hello = argv[1];
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("\nScoket creation error\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("\nInvalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }

    if((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))<0) {
        perror("\nConnection Failed\n");
        exit(EXIT_FAILURE);
    }

    send(client_fd, hello, strlen(hello), 0);
    printf("# Sent Message: %s\n", hello);
    valread = read(client_fd, buffer, 1024-1); // subtract 1 for the null terminator at the end
    printf("# Received Message: %s\n", buffer);

    // closing the connected socket
    close(client_fd);

    return 0;
}

