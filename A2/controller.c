#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

const int opt_enable = 1;
const int buffer_size = 1023;
const int port = 3000;
const socklen_t socklen = sizeof(struct sockaddr_in);
const socklen_t clientlen = sizeof(struct sockaddr);

/**
 * Handles routing of cars requested by call pads.
 * Acts as a TCP server.
 */
int main(void)
{
    /*Declare variables*/
    int sockfd; // Listening socket file descriptor.
    int clientfd; // Client file descriptor.

    int bytesrcv; // Incoming bytes.
    char buffer[buffer_size]; // Receiving buffer.

    struct sockaddr_in server_addr;
    struct sockaddr client_addr;

    /*Initialise listening socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "Unable to create socket.");
        return EXIT_FAILURE;
    }

    /*Initialise server data structure*/
    memset(&server_addr, '\0', socklen);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    /*Enable address reuse*/
    setsocketopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));

    /*Bind socket*/
    if (bind(sockfd, (struct sockaddr*)&server_addr, socklen) == -1)
    {
        fprintf(stderr, "Unable to bind socket.");
        return EXIT_FAILURE;
    }

    /*Listen for incomming requests from cars or call pads*/
    if (listen(sockfd, 10) == -1)
    {
        fprintf(stderr, "Unable to listen for clients.")
    }

    while (1)
    {
        /*Accept an incoming client request*/
        clientfd = accept(sockfd, &client_addr, &clientlen);
        if (clientfd == -1)
        {
            fprintf(stderr, "Unable to accept client.");
            return EXIT_FAILURE;
        }

        /*Get client data*/
        bytesrcv = recv(clientfd, buffer, buffer_size, 0);
        if (bytesrcv == -1)
        {
            fprintf(stderr, "Unable to receive client data.");
        }
        buffer[bytesrcv] = '\0'; // Add null terminator.

        close(clientfd); // Close socket.
    }

    if (shutdown(clientfd, SHUT_RDWR) == -1)
    {
        fprintf(stderr, "Unable to shut down client.");
        return EXIT_FAILURE;
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
