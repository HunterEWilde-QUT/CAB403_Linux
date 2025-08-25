#include <pthread.h>
#include <stdio.h>
#include <string.h>    /* String handling */

int globalVar = 0;
pthread_mutex_t mutex;

void *addMillion(void *arg);

int main(int argc, char *argv[]) {
    pthread_t thread1, thread2, thread3;
    pthread_t threads[] = {thread1, thread2, thread3};
    pthread_mutex_init(&mutex, NULL); // Initialise mutex lock
    for(int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, addMillion, NULL);
    }
    for(int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("\n\nTotal for globalVar = %d\n", globalVar);
    return 0;
}

void *addMillion(void *ptr) {
    if(pthread_mutex_lock(&mutex) != 0) {
        perror("Failed to create mutex lock.");
        return NULL;
    }
    for (int i = 0; i < 10000000; i++) {
        globalVar++;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
