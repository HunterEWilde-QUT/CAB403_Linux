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
 
int main(int argc, char **argv){
	/* check port number is passed in in on command line */			
    if(argc != 2) {
        fprintf(stderr, "\nUnable to bind socket.\n");
        return EXIT_FAILURE;
    }
	/* variables required for solution */
	int sockfd;	
	struct sockaddr_in server_addr, client_addr;
	char buffer[BUFFER_SIZE];
	socklen_t addr_size;
	char *ip = "127.0.0.1";
	bool cont = true;

    int port = atoi(argv[1]);

	/* Create a UDP Socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	/* Initialise the sockaddr_in structure 
	 * use memset to zero the struct out
	 */
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

	/* bind server address to socket descriptor */
	bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));

    while (cont) {
		/* clear buffer */
        bzero(buffer, BUFFER_SIZE);
		/* Calculating the Client Datagram length */
		addr_size = sizeof(client_addr);
		/* use recvfrom to receive data from client */
		int brcv = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &client_addr, &addr_size);
        buffer[brcv] = '\0';

		/* If termination message sent from server exit while loop */
		if(strncasecmp(buffer, "q", 1) == 0) {
            cont = false;
            printf("\nReceived exit message. Closing socket.\n");
        } else {
            /* Output the message from the client */
            printf("\nReceived %d bytes from client was %s.", brcv, buffer);
            /* Clear the buffer and send a message from the server to the client */
            bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "\nMessage received. Have a nice day.\n");
            sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &client_addr, addr_size);
        }
	}
	// Closing the Socket File Descriptor.
	close(sockfd);
    shutdown(sockfd, SHUT_RDWR);
	return 0;
}
