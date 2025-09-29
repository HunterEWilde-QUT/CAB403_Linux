#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "internal.h"
#include "status.h"

const char *str_car = "car";
const size_t shmem_size = sizeof(car_shared_mem);

void car_init(car_shmem_ctrl *, char *);

/**
 * Creates & initialises a car with a shared memory object.
 */
int main(int argc, char *argv[]) {
    /*Handle invalid usage*/
    if (argc != 5) {
        fprintf(stderr, "Usage: ./car {name} {lowest floor} {highest floor} {delay}");
        return EXIT_FAILURE;
    }

    /*Declare variables*/
    car_shmem_ctrl *car; // Shared memory control struct
    char *name;          // Name of elevator car (e.g. "A", "Service", "Test")
    /**
     * Range of accessible floors
	 * Must be exactly 3 characters + '\0' ("B99"-"999")
	 */
	char floor_min[4];
    char floor_max[4];
    int delay;           // Operation delay in milliseconds

    /*Get required input arguments*/
    strcpy(name, argv[1]);
    strcpy(floor_min, argv[2]);
    strcpy(floor_max, argv[3]);
    delay = atoi(argv[4]);

    /*Initialise car shared memory*/
    car = malloc(sizeof(car_shmem_ctrl)); // Memory must be allocated for this shared object
    car_init(car, &name);

	/*Initialise car data from input args*/
	strcpy(car->data->current_floor, floor_min);
	strcpy(car->data->destination_floor, floor_min);
	strcpy(car->data->status, str_closed);

    return EXIT_SUCCESS;
}

/**
 * Creates shared memory structure & initialises modes.
 * @param car Shared mem control struct for a car; contains car data struct.
 * @param name String to be appended to the designation "car" to form shared mem object name.
 */
void car_init(car_shmem_ctrl *car, char *name) {
    /*Assemble car name*/
    strcpy(car->name, str_car);
    strcat(car->name, name);

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
    pthread_mutexattr_init(&mutex_attr); // Initialise mutex attribute
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED); // Enable pshared

    pthread_mutex_init(&car->mutex, &mutex_attr); // Initialise mutex with pshared enabled

    /*Configure condition*/
    pthread_condattr_t cond_attr; // Attribute object needed to initialise the condition
    pthread_condattr_init(&cond_attr); // Initialise condition attribute
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED); // Enable pshared

    pthread_cond_int(&car->cond, &cond_attr); // Initialise cond with pshared enabled

    /*Initialise boolean modes as false*/
    car->data->open_button = 0;
    car->data->close_button = 0;
    car->data->safety_system = 0;
    car->data->door_obstruction = 0;
    car->data->overload = 0;
    car->data->emergency_stop = 0;
    car->data->individual_service_mode = 0;
    car->data->emergency_mode = 0;
}
