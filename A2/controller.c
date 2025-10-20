#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "tcp.h"

const int opt_enable = 1;
const int BUFFERLEN = 1023;

/**
 * For creating linked list of threads for incoming TCP requests.
 * Facilitates tracking of all threads so that they may be joined neatly upon program completion.
 */
typedef struct tcp_thread_node_struct tcp_thread_node;
struct tcp_thread_node_struct
{
	pthread_t thread;		// Thread object to be created for handling client request (i.e. car or call pad)
	uint8_t thread_created;	// Boolean to track if this thread has been created yet
	tcp_thread_node* next;	// Pointer to next node in linked list
};

pthread_t link_thread_node(tcp_thread_node*);
void* handle_connection(void*);
void connect_car(const int*, char*);
void send_car(char*);

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

	/*Initialise a linked list for car threads*/
	tcp_thread_node* head;
	head->thread_created = 0;
	head->next = NULL;

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
		pthread_t current_thread = link_thread_node(head);
		if (pthread_create(&current_thread, NULL, handle_connection, clientfd) != 0)
		{
			fprintf(stderr, "\nUnable to create controller thread.\n");
			return EXIT_FAILURE;
		}
		pthread_detach(current_thread); // Allow thread to self-terminate when finished

		//break;
    }

    return EXIT_SUCCESS;
}

/**
 * Adds a new TCP thread node to the linked list.
 * @param node Node in the linked list.
 * @return Thread object of the new node.
 */
pthread_t link_thread_node(tcp_thread_node* node)
{
	if (!node->thread_created) // This node hasn't been used yet (only true for head node)
	{
		node->thread_created = 1;
		return node->thread;
	}
	if (node->next == NULL) // This is the tail node; a new node is needed
	{
		tcp_thread_node* new;	// Create a new node
		node->next = new;		// Link this node to the new node
		new->next = NULL;		// New node is the new tail
		new->thread_created = 1;
		return new->thread;
	}
	// This node is not the tail; must traverse linked list
	return link_thread_node(node->next);
}

/**
 * Reads the message in an incoming TCP request & conditionally calls the relevant function to handle it.
 * @param ptr Client file descriptor: identifies the connection to a car or call pad.
 */
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
	if (strstr(buffer, "CAR") != NULL) // A new car has been created
	{
		connect_car(clientfd, buffer);
	}
	else if (strstr(buffer, "CALL") != NULL) // A call pad is summoning a car
	{
		send_car(buffer);
	}

	return NULL;
}

/**
 * Register a new car & handle 2-way communication.
 */
void connect_car(const int* clientfd, char* buffer)
{
	/*Add car to register*/


	/*Terminate connection in case of service mode or emergency*/
	if (strstr(buffer, "INDIVIDUAL SERVICE") != NULL
		|| strstr(buffer, "EMERGENCY") != NULL)
	{
		// Not sure if this is needed
		// if (shutdown(clientfd, SHUT_RDWR) == -1)
		// {
		// 	fprintf(stderr, "\nUnable to shut down client.\n");
		// 	return EXIT_FAILURE;
		// }
		close(*clientfd); // Close socket
	}
}

/**
 * Process a request from a call pad & add its source floor to a registered car's queue.
 */
void send_car(char* buffer)
{

}
