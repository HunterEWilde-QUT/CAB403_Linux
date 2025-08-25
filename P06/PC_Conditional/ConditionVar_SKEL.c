#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define MAX_ITEMS 5

int buffer[BUFFER_SIZE] = {0};
int in = 0;
int out = 0;
int produced_count = 0;
int consumed_count = 0;

pthread_mutex_t mutex;
pthread_cond_t full, empty;

void printBuffer();
void* producer(void* arg);
void* consumer(void* arg);

int main(int argc, char *argv[]) {
    int producerThreads = 2;
    int consumerThreads = 2;
    pthread_t pThreads[producerThreads];
    pthread_t cThreads[consumerThreads];
    // mutex not used as yet
    pthread_mutex_init(&mutex, NULL);
    for(int i = 0; i < producerThreads; i++) {
        pthread_create(&pThreads[i], NULL, producer, NULL);
    }
    for(int i = 0; i < consumerThreads; i++) {
        pthread_create(&cThreads[i], NULL, consumer, NULL);
    }

    for(int i = 0; i < producerThreads; i++) {
        pthread_join(pThreads[i], NULL);
    }
    for(int i = 0; i < consumerThreads; i++) {
        pthread_join(cThreads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);

    return 0;
}

void printBuffer() {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf(" %d ", buffer[i]);
    }
    printf("\n");
}

void* producer(void* arg) {
    for (;;) {
        pthread_mutex_lock(&mutex);
        while (((in + 1) % BUFFER_SIZE) == out) {
            pthread_cond_wait(&empty, &mutex);
        }
        buffer[in] = 1;
        in = (in + 1) % BUFFER_SIZE;
        pthread_cond_signal(&full);
        printBuffer();
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

void* consumer(void* arg) {
    for (;;) {
        pthread_mutex_lock(&mutex);
        while (in == out) {
            pthread_cond_wait(&full, &mutex);
        }
        buffer[out] = 0;
        out = (out + 1) % BUFFER_SIZE;
        pthread_cond_signal(&empty);
        printBuffer();
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}
