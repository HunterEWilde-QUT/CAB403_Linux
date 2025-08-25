#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TRUE 1

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int insertPointer = 0, removePointer = 0;

void *producer(void *param);
void *consumer(void *param);

int insert_item(buffer_item item) {
	//Acquire Empty Semaphore
	sem_wait(&empty);

	//Acquire mutex lock to protect buffer
	pthread_mutex_lock(&mutex);
	buffer[insertPointer++] = item;
	insertPointer = insertPointer % BUFFER_SIZE;

	//Release mutex lock and full semaphore
	pthread_mutex_unlock(&mutex);
	sem_post(&full);

	return 0;
}

int remove_item(buffer_item *item) {
	/* insert your code here for Task 1 (a) */
	/* Acquire Full Semaphore */
    sem_wait(&full);

	/* Acquire mutex lock to protect buffer */
    pthread_mutex_lock(&mutex);
    *item = buffer[removePointer];
    buffer[removePointer++] = -1;
    removePointer = removePointer % BUFFER_SIZE;

	/* Release mutex lock and empty semaphore */
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);

	return 0;
}

int main(int argc, char *argv[]) {
	int sleepTime, producerThreads, consumerThreads;
	//int i, j;

	if (argc != 4) 	{
		fprintf(stderr, "Useage: <sleep time> <producer threads> <consumer threads>\n");
		return -1;
	}
    /* insert you code below for Task 1 (b) */
	/* Get command line arguments argv[1], argv[2], argv[3]*/
    producerThreads = atoi(argv[1]);
    consumerThreads = atoi(argv[2]);
    sleepTime = atoi(argv[3]);

    pthread_t pthreads[producerThreads];
    pthread_t cthreads[consumerThreads];

	/* Initialize the the locks */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    srand(time(0));

    int valOne, valTwo;
    sem_getvalue(&full, &valOne);
    sem_getvalue(&empty, &valTwo);
    printf("\nThe value of full semaphore is %d\n", valOne);
    printf("\nThe value of empty semaphore is %d\n", valTwo);

	/* Create the producer and consumer threads */
    for(int i = 0; i < producerThreads; i++) {
        pthread_create(&pthreads[i], NULL, producer, NULL);
    }
    for(int i = 0; i < consumerThreads; i++) {
        pthread_create(&cthreads[i], NULL, consumer, NULL);
    }
    sleep(sleepTime);
    for(int i = 0; i < producerThreads; i++) {
        pthread_join(pthreads[i], NULL);
    }
    for(int i = 0; i < consumerThreads; i++) {
        pthread_join(cthreads[i], NULL);
    }
	/* Sleep for user specified time */

	return 0;
}

void *producer(void *param) {
	buffer_item random;
	int r;

	while (TRUE) 	{
		r = rand() % 5;
		sleep(r);
		random = rand();

		if (insert_item(random)) {
			fprintf(stderr, "Error");
		} else {
			printf("Producer produced %d \n", random);
		}
	}
}

void *consumer(void *param) {
	buffer_item random;
	int r;

	while (TRUE) 	{
		r = rand() % 5;
		sleep(r);

		if (remove_item(&random)) {
			fprintf(stderr, "Error Consuming");
		} else {
			printf("Consumer consumed %d \n", random);
		}
	}
}
