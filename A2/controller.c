#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "tcp.h"

const int opt_enable = 1;
const int BUFFERLEN = 1023;

typedef struct
{
	pthread_t thread;
	tcp_thread_node* next;
} tcp_thread_node;

void* handle_connection(void*);	// Thread to handle incomming requests
void connect_car(void);			// Car request logic
void connect_call(void);		// Call pad request logic

/**
 * Handles routing of cars requested by call pads.
 * Acts as a TCP server.
 */
int main(void)
{
    /*Declare variables*/
    int sockfd; // Listening socket file descriptor
    int clientfd; // Client file descriptor

    struct sockaddr_in server_addr;
    struct sockaddr client_addr;

	/*Create a linked list for car threads*/
	tcp_thread_node* head;
	car_thread->next = NULL;

    /*Initialise listening socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "\nUnable to create socket.\n");
        return EXIT_FAILURE;
    }

    /*Initialise server data structure*/
    memset(&server_addr, '\0', SOCKLEN);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    /*Enable address reuse*/
    setsocketopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));

    /*Bind socket*/
    if (bind(sockfd, (struct sockaddr*)&server_addr, SOCKLEN) == -1)
    {
        fprintf(stderr, "\nUnable to bind socket.\n");
        return EXIT_FAILURE;
    }

    /*Listen for incomming requests from cars or call pads*/
    if (listen(sockfd, 10) == -1)
    {
        fprintf(stderr, "\nUnable to listen for clients.\n");
    }

	while (1)
    {
        /*Accept an incoming client request*/
        clientfd = accept(sockfd, &client_addr, &CLIENTLEN);
        if (clientfd == -1)
        {
            fprintf(stderr, "\nUnable to accept client.\n");
            return EXIT_FAILURE;
        }

		/*Create thread to handle request*/
		if (pthread_create(&node->thread, NULL, handle_connection, clientfd) != 0)
		{
			fprintf(stderr, "\nUnable to create controller thread.\n");
			return EXIT_FAILURE;
		}

		//break;
    }

	/*Terminate threads*/
	//pthread_join(head->thread, NULL);
	//if (head->next != NULL)
	//{
	//
	//}

    if (shutdown(clientfd, SHUT_RDWR) == -1)
    {
        fprintf(stderr, "\nUnable to shut down client.\n");
        return EXIT_FAILURE;
    }

    close(sockfd);

    return EXIT_SUCCESS;
}

void* handle_connection(void* ptr)
{
	const int* clientfd = ptr;
	int bytesrcv; 			// Incoming bytes
    char buffer[BUFFERLEN]; // Receiving buffer

	/*Get client data*/
    bytesrcv = recv(clientfd, buffer, BUFFERLEN, 0);
    if (bytesrcv == -1)
    {
        fprintf(stderr, "\nUnable to receive client data.\n");
    }
    buffer[bytesrcv] = '\0'; // Add null terminator

	/*Direct to relevant handler*/

}

void connect_car(void)
{

	close(clientfd); // Close socket
}

void connect_call(void)
{

}
