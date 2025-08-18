#include <unistd.h>    /* Symbolic Constants */
#include <sys/types.h> /* Primitive System Data Types */
#include <errno.h>     /* Errors */
#include <stdio.h>     /* Input/Output */
#include <stdlib.h>    /* General Utilities */
#include <pthread.h>   /* POSIX Threads */
#include <string.h>    /* String handling */

#define MESSAGE_REPEAT 2

/* struct to hold data to be passed to a thread
   this shows how multiple data items can be passed to a thread */
typedef struct str_thdata {
    int thread_no;
    char message[100];
} thdata;

typedef struct num_thdata {
	int thread_no;
	int sum_to;
} thsum;

/* prototype for thread routine */
void *print_message_function(void *ptr);
void *sum_numbers(void *ptr);

int main(int argc, char *argv[]) {
    pthread_t thread1, thread2, thread3; /* thread variables */
    thdata data1, data2, data3; /* structs to be passed to threads */

    /* initialize data to pass to thread 1 */
    data1.thread_no = 1;
    strcpy(data1.message, "Hello! Welcome to Practical 5 - Week 5 already!!!");

    /* initialize data to pass to thread 2 */
    data2.thread_no = 2;
    strcpy(data2.message, "Hi! Week 5 - Time flies by when programming in C");

	/* initialize data to pass to thread 3 */
	data3.thread_no = 3;
	strcpy(data3.message, "I created a third thread.");

    /* create threads 1 and 2 
    *  function must take a parameter of void *(the second void *)
    *  return a value of void * (first void)
    */
    if(pthread_create(&thread1, NULL, print_message_function, &data1) != 0) {
		printf("Error creating thread 1\n");
		return EXIT_FAILURE;
	}
    if(pthread_create(&thread2, NULL, print_message_function, &data2) != 0) {
		printf("Error creating thread 2\n");
		return EXIT_FAILURE;
	}
	if(pthread_create(&thread3, NULL, print_message_function, &data3) != 0) {
		printf("Error creating thread 3\n");
		return EXIT_FAILURE;
	}

    /* Main block now waits for both threads to terminate, before it exits
       If main block exits, both threads exit, even if the threads have not
       finished their work */
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

    /* exit */
    return EXIT_SUCCESS;
} /* main() */

/**
 * print_message_function is used as the start routine for the threads used
 * it accepts a void pointer 
**/
void *print_message_function(void *ptr) {
    thdata *data;
    data = ptr; /* type cast to a pointer to thdata */

    /* do the work */
    for (int x = 0; x < MESSAGE_REPEAT; x++)  {
        printf("\n\nThread %d has the following message -->  %s \n", data->thread_no, data->message);
    }
    return NULL;
}

void *sum_numbers(void *ptr) {
	thdata *data;
	data = ptr;

	int sum;
	sum = 0;

	printf("%s", data->message);
	for(int i = 0; i <= data->thread_no; i++) {
		sum += i;
	}
	printf("Thread number 3 reports that the sum of the first 20 numbers is = %d", sum);
	return NULL;
}
