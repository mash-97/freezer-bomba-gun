#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
    int server_fd;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd<0) {
        perror("Socket Error!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
    socklen_t server_addr_len = sizeof(server_addr);

    int binded = bind(server_fd, (struct sockaddr*)&server_addr, server_addr_len);
    if(binded<0) {
        perror("Socket binding failed!");
        exit(EXIT_FAILURE);
    }

    int listening = listen(server_fd, 3);
    if(listening<0) {
        perror("Listening failed!");
        exit(EXIT_FAILURE);
    }

    while(1) {
        int client_fd = accept(server_fd,  (struct sockaddr *)&server_addr, &server_addr_len);
        if(client_fd<0) {
            perror("Client accept failed!");
            exit(EXIT_FAILURE);
        }
        
        char buffer[1024]={0};
        ssize_t read_size = read(client_fd, buffer, 1024-1);
        printf("Received Message: %s\n", buffer);

        send(client_fd, "Hello", strlen("Hello"), 0);
        close(client_fd);
    }
    
    close(server_fd);

    return 0;
}

   
