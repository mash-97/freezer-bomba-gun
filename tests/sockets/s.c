// Server side C/C++ program to demonstrate Socket
// Programming
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080


void printSocketAddressInternet(struct sockaddr_in sockaddr) {
	printf("\n");
	printf("Address sin_family: %hu\n", sockaddr.sin_family);
	printf("Address sin_addr.s_addr: %u\n", sockaddr.sin_addr.s_addr);
	printf("IP address: %s\n", inet_ntoa(sockaddr.sin_addr));
	printf("Address sin_port: %hu\n", sockaddr.sin_port);
	printf("\n");
}

int main(int argc, char **argv) {
  int server_fd, new_socket;  // server socket file descriptor, also for the client
  ssize_t valread;
  struct sockaddr_in address;
  int opt = 1;
  socklen_t addrlen = sizeof(address);
  const int BUFF_SIZE = 1024*30;
  char buffer[1024*30] = {0};
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
	printSocketAddressInternet(address);

  // Forcefully attaching socket to the port 8080
  if(bind(server_fd, (struct sockaddr*) &address, sizeof(address))<0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("binded\n");
  printSocketAddressInternet(address);

  if(listen(server_fd, 3)<0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  printf("listening\n");
  printSocketAddressInternet(address);

	while(1) {
  	if((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
    	perror("accept");
    	exit(EXIT_FAILURE);
  	}
  	printf("=> Accepted: %u\n", new_socket);
  	printSocketAddressInternet(address);

		valread = read(new_socket, buffer, BUFF_SIZE-1); // subtract 1 for the null terminator at the end
		printf("# Message Recieved: %s, len: %d:%ld\n", buffer, (int)strlen(buffer), valread);
  	valread = send(new_socket, hello, strlen(hello), 0);
  	printf("# Message Sent: %s, len: %d:%ld\n", hello, (int)strlen(hello), valread);
  	if(strcmp(buffer, "break")==0){printf("breaking...\n");break;}
  	// closing the current socket
  	close(new_socket);
	}

  // closing the listening socket
  close(server_fd);

  return 0;
}


