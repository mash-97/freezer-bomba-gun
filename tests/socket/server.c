// Server side C/C++ program to demonstrate Socket
// Programming
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080

int main(int argc, char **argv) {
  int server_fd, new_socket;  // server socket file descriptor, also for the client
  ssize_t valread;
  struct sockaddr_in address;
  int opt = 1;
  socklen_t addrlen = sizeof(address);
  char buffer[1024] = {0};
  char *hello = "Hello from server";
if(argc>1) hello = argv[1];
  // Creating socket file descriptor
  if(( server_fd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if(bind(server_fd, (struct sockaddr*) &address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
  printf("binded\n");

  if(listen(server_fd, 3)<0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  printf("listening\n");
while(1) {
  if((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  
  valread = read(new_socket, buffer, 1024-1); // subtract 1 for the null terminator at the end
  printf("# Message Recieved: %s\n", buffer);
  send(new_socket, hello, strlen(hello), 0);
  printf("# Message Sent: %s, len: %d\n", hello, (int)strlen(hello));
  if(strcmp(buffer, "break")==0){printf("breaking...\n");break;}
}
  // closing the connected socket
  close(new_socket);

  // closing the listening socket
  close(server_fd);

  return 0;
}


