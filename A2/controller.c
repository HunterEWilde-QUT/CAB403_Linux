#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "controller.h"
#include "tcp.h"

car_register* car_head; // Global scope is needed to be accessible by multiple threads

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

	/*Initialise linked list for car threads*/
	tcp_thread_node* tcp_head;
	tcp_head->thread_created = 0;
	tcp_head->next = NULL;

	/*Initialise linked list for active cars register*/
	car_register_init(car_head);

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
		pthread_t current_thread = link_thread_node(tcp_head);
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
 * Initialise a new node in the linked list of active cars.
 * @param node Pointer to a node in the linked list.
 */
void car_register_init(car_register* node)
{
	/*Configure mutex lock*/
	pthread_mutexattr_t mutex_attr; // Attribute object needed to initialise the mutex
	pthread_mutexattr_init(&mutex_attr); // Initialise mutex attribute
	pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED); // Enable pshared

	pthread_mutex_init(&node->mutex, &mutex_attr); // Initialise mutex with pshared enabled

	pthread_mutexattr_destroy(&mutex_attr);

	/*Configure condition*/
	pthread_condattr_t cond_attr; // Attribute object needed to initialise the condition
	pthread_condattr_init(&cond_attr); // Initialise condition attribute
	pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED); // Enable pshared

	pthread_cond_init(&node->cond, &cond_attr); // Initialise cond with pshared enabled

	pthread_condattr_destroy(&cond_attr);

	/*Initialise linking properties*/
	node->name = NULL; // Probably redundant, but explicitness doesn't hurt
	node->next = NULL;
}

/**
 * Provides a usable node at the tail of the linked list of active cars.
 * Creates & initialises a new node if needed.
 * @param node Node in the linked list.
 * @return Tail of linked list; usable node.
 */
car_register* link_car_node(car_register* node)
{
	pthread_mutex_lock(&node->mutex);	// Lock mem struct

	if (node->name == NULL) // This node hasn't been used yet
	{
		return node; // Use this node
	}
	if (node->next == NULL) // This is the tail node; a new node is needed
	{
		car_register* new;
		car_register_init(new);

		node->next = new; // Link new node to the existing list

		return new; // Use the new node
	}

	//pthread_cond_signal(&node->cond);	// Signal contents changed
	pthread_mutex_unlock(&node->mutex);	// Unlock mem struct

	// This node is not the tail; must traverse linked list
	return link_car_node(node->next);
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
 * @param fd Client file descriptor.
 * @param buffer Message string from client.
 */
void connect_car(const int* fd, char* buffer)
{
	/*Store car properties*/
	car_register* car = link_car_node(car_head); // Acquire usable car register from linked list

	strcpy(car->name, strtok(buffer, ' '));
	strcpy(car->min_floor, strtok(NULL, ' '));
	strcpy(car->max_floor, strtok(NULL, ' '));

	/*Maintain communication with car*/
	while (1)
	{
		/*Receive most recent message*/
		buffer = get_client_data(fd);

		/*Handle car status update*/
		if (strstr(buffer, "STATUS") != NULL)
		{
			strcpy(car->status, strtok(buffer, ' '));
			strcpy(car->current_floor, strtok(buffer, ' '));
			strcpy(car->destination_floor, strtok(buffer, ' '));
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

			break;
		}
	}

	close(*fd); // Close socket
}

/**
 * Process a request from a call pad & add its source floor to a registered car's queue.
 */
void send_car(char* buffer)
{

}
