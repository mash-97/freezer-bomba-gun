#include <stdio.h>
#include <arpa/inet.h>

int main() {
    unsigned short host_port = 12345;
    unsigned short network_port = htons(host_port);

    printf("Host byte order: %hu\n", host_port);
    printf("Network byte order (htons): %hu\n", network_port);

    return 0;
}
