#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "controller.h"
#include "tcp.h"

/**
 * Handles routing of cars requested by call pads.
 * Acts as a TCP server.
 */
int main(void)
{
    /*Initialise listening socket*/
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "\nUnable to create socket.\n");

        return EXIT_FAILURE;
    }

    /*Initialise server data structure*/
	struct sockaddr_in server_addr;

    memset(&server_addr, '\0', SOCKLEN);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    /*Enable address reuse*/
    setsocketopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &OPT_ENABLE, sizeof(OPT_ENABLE));

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

	/*Connect to clients (i.e. cars or call pads)*/
	int clientfd;
	struct sockaddr client_addr;

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
		if (pthread_create(&current_thread, NULL, handle_connection, &clientfd) != 0)
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
 * Receive data from a client.
 * @param fd File descriptor of the client.
 * @return Buffer containing the client's message.
 */
char* get_client_data(const int* fd)
{
	char buffer[BUFFERLEN]; // Receiving buffer

	int bytesrcv = recv(fd, buffer, BUFFERLEN, 0);
	if (bytesrcv == -1)
	{
		fprintf(stderr, "\nUnable to receive client data.\n");
	}

	buffer[bytesrcv] = '\0'; // Add null terminator

	return buffer;
}

/**
 * Reads the message in an incoming TCP request & conditionally calls the relevant function to handle it.
 * @param ptr Client file descriptor: identifies the connection to a car or call pad.
 */
void* handle_connection(void* ptr)
{
	const int* fd = ptr;
    char* buffer = get_client_data(fd);

	/*Direct to relevant handler*/
	if (strstr(buffer, "CAR") != NULL) // A new car has been created
	{
		connect_car(fd, buffer);
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
void connect_car(const int* fd, char* buffer)
{
	/*Get car properties*/
	char* name = strtok(buffer, ' ');
	char min_floor[4] = strtok(NULL, ' ');
	char max_floor[4] = strtok(NULL, ' ');

	/*Maintain communication with car*/
	while (1)
	{
		/*Receive most recent message*/
		buffer = get_client_data(fd);

		/*Handle car status update*/
		if (strstr(buffer, "STATUS") != NULL)
		{

		}

		/*Terminate connection in case of service mode or emergency*/
		if (strstr(buffer, "INDIVIDUAL SERVICE") != NULL
			|| strstr(buffer, "EMERGENCY") != NULL)
		{
			// Not sure if this is needed
			// if (shutdown(fd, SHUT_RDWR) == -1)
			// {
			// 	fprintf(stderr, "\nUnable to shut down client.\n");
			// 	return EXIT_FAILURE;
			// }
			close(*fd); // Close socket

			break;
		}
	}
}

/**
 * Process a request from a call pad & add its source floor to a registered car's queue.
 */
void send_car(char* buffer)
{

}
