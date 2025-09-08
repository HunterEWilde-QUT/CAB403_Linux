#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZE 1023

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "\nUnable to bind socket.");
        return EXIT_FAILURE;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("\nsocket()\n");
        return 1;
    }

    struct sockaddr_in servAddr;
    memset(&servAddr, '\0', sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if(bind(fd, (struct sockaddr*) &servAddr, sizeof(servAddr)) == -1) {
        perror("\nbind()\n");
        return 1;
    } else {
        printf("\nSocket bound.\n");
    }
}
