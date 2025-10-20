#pragma once

const int OPT_ENABLE = 1;
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

/*Keep track of active cars*/
typedef struct
{
	pthread_mutex_t mutex;  // Locked while accessing struct contents
    pthread_cond_t cond;    // Signalled when the contents change
    char* name;             // Car name
    char min_floor[4];      // Lowest accessible floor
    char max_floor[4];      // Highest accessible floor
} car_register;

pthread_t link_thread_node(tcp_thread_node*);
char* get_client_data(const int*);
void* handle_connection(void*);
void connect_car(const int*, char*);
void send_car(char*);
