#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#define BUFFER_SIZE 5
#define MAX_SLEEP 5.0

typedef struct message_queue {
    time_t buffer[BUFFER_SIZE];
    int keep_running;
    int position;
    int is_room;
} mq;

/* function prototypes */
void *producer_function(void *);
void *consumer_function(void *);

int main(int argc, char *argv[]) {
    int producer_number;
    int consumer_number;
    if (argc != 3) {
        fprintf(stderr, "Incorrect usage: ./app n n\n");
        return EXIT_FAILURE;
    } else {
        producer_number = atoi(argv[1]);
        consumer_number = atoi(argv[2]);
    }

    char ignore_this[80];
    
    printf("\nHit the enter key to stop the threads executing\n\n");
    sleep(2);
    
    srand((unsigned) time(NULL)); /* seed the random number generator.*/

    mq messages;
    messages.keep_running = true;
    messages.position = 0;
    messages.is_room = true;

    pthread_t producers[producer_number];
    pthread_t consumers[consumer_number];
	for (int i = 0; i < producer_number; i++) {
		if (pthread_create(&producers[i], NULL, producer_function, &messages)) {
		    fprintf(stderr, "Error creating producer thread\n");
		    return EXIT_FAILURE;
		}
	}
	for (int i = 0; i < consumer_number; i++) {
		if (pthread_create(&consumers[i], NULL, consumer_function, &messages)) {
		    fprintf(stderr, "Error creating consumer thread\n");
            return EXIT_FAILURE;
		}
	}

    fgets(ignore_this, 80, stdin);
    messages.keep_running = false;
    for(int i = 0; i < producer_number; i++) {
        if (pthread_join(producers[i], NULL)) {
            fprintf(stderr, "Error joining producer thread\n");
            return EXIT_FAILURE;
        }
    }
    for(int i = 0; i < consumer_number; i++) {
        if (pthread_join(consumers[i], NULL)) {
            fprintf(stderr, "Error joining consumer thread\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

void *producer_function(void *arg) {
    mq *mqueue = arg;
    int sleeptime;
    time_t message;

    while (mqueue->keep_running) {
        sleeptime = (int)(MAX_SLEEP * rand() / (RAND_MAX + 1.0));
        printf("Producer sleeping for %d seconds\n", sleeptime);
        fflush(stdout);
        sleep(sleeptime);

        message = time(NULL);
        printf("Producer produced %s", ctime(&message));
        /* this is the blocking send */

        while (!mqueue->is_room) {
            /* no room */
            printf("Producer Blocked\n");
            fflush(stdout);
            sleep(1);
        }

        printf("Producer UnBlocked\n");
        fflush(stdout);

        mqueue->buffer[mqueue->position++] = message;

        printf("\t\t\t\tNumber of items in buffer: %d\n", mqueue->position);
        fflush(stdout);

        if (mqueue->position == BUFFER_SIZE) {
            mqueue->is_room = false;
            printf("Buffer is Full\n");
            fflush(stdout);
        }
    }
    printf("Producer told to stop.\n");
    fflush(stdout);
    return NULL;
}

void *consumer_function(void *arg) {
    mq *mqueue = arg;
    time_t message;
    int sleeptime;

    while (mqueue->keep_running) {
        sleeptime = (int)(MAX_SLEEP * rand() / (RAND_MAX + 1.0));

        printf("\t\tConsumer sleeping for %d seconds\n", sleeptime);
        fflush(stdout);
        sleep(sleeptime);
        /* this is the blocking receive */

        printf("\t\tConsumer wants to consume\n");
        fflush(stdout);

        while (mqueue->position) {
            /* nothing there so I have to wait */
            printf("\t\tConsumer Blocked\n");
            fflush(stdout);
            sleep(1);
        }

        printf("\t\tConsumer UnBlocked\n");
        fflush(stdout);

        message = mqueue->buffer[--mqueue->position];
        mqueue->is_room = true;

        printf("\t\tConsumer consumed %s", ctime(&message));
        printf("\t\t\t\tNumber of items in buffer: %d\n", mqueue->position);
        fflush(stdout);
    }
    printf("\t\tConsumer told to stop.\n");
    fflush(stdout);
    return NULL;
}
