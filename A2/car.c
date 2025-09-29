#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "internal.h"

const char *car_ = "car";
const size_t shmem_size = sizeof(car_shared_mem);

void car_init(car_shmem_ctrl *, char *);

/**
 * Justify any MISRA-C violations
 */
int main(int argc, char *argv[]) {
    /*Declare variables*/
    car_shmem_ctrl *car; // Shared memory control struct
    char *name;          // Name of elevator car (e.g. "A")
    char *floor_min;     // Lowest accessible floor (B99-999)
    char *floor_max;     // Highest accessible floor (B99-999)
    int delay;           // Operation delay in milliseconds

    /*Get required input arguments*/
    if (argc != 5) {
        fprintf(stderr, "Usage: ./car {name} {lowest floor} {highest floor} {delay}");
        return EXIT_FAILURE;
    }
    name = atoi(argv[1]);
    floor_min = atoi(argv[2]);
    floor_max = atoi(argv[3]);
    delay = atoi(argv[4]);

    /*Initialise car shared memory*/
    car = malloc(sizeof(car_shmem_ctrl)); // Memory must be allocated for this shared object
    car_init(car, &name);

    return EXIT_SUCCESS;
}

void car_init(car_shmem_ctrl *car, char *name) {
    /*Assemble car name*/
    strcpy(car->name, car_);
    strcat(car-name, name);

    /*Remove any previous instances*/
    shm_unlink(car->name);

    /*Create a new instance of this shared memory object*/
    car->fd = shm_open(object, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (car->fd == -1) {
        car->data = NULL;
        fprintf(stderr, "Unable to create shared memory object.");
        return;
    }
    if (ftruncate(car->fd, shmem_size) == -1) {
        car->data = NULL;
        fprintf(stderr, "Unable to set capacity of shared memory object.");
        return;
    }
    car->data = mmap(NULL, shmem_sizel, PROT_READ | PROT_WRITE, MAP_SHARED, car->fd, 0);
    if (car->data == MAP_FAILED) {
        fprintf(stderr, "Unable to map shared memory.");
        return;
    }

    /*Configure mutex lock*/
    pthread_mutexattr_t mutex_attr; // Attribute object needed to initialise the mutex
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&car->mutex, &mutex_attr);

    /*Configure condition*/
    pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    //setpshared

    pthread_cond_int(&car->cond, &cond_attr);
}
