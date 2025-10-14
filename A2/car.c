#include <arpa/inet.h>
#include <unistd.h>
#include <sys/mman.h>
#include "internal.h"
#include "tcp.h"
#include "keywords.h"

const size_t shmem_size = sizeof(car_shared_mem);
uint16_t delay; // Operation delay in milliseconds

void car_init(car_shmem_ctrl*, char*);
void car_kill(car_shmem_ctrl*);

/*Use pointers for multithreading*/
void* connect_controller(void*);
void* car_run(void*);

/**
 * Creates & initialises a car with a shared memory object.
 * @param argc Number of command line arguments: 1.
 * @param argv name, lowest_floor, highest_floor, delay.
 * @return Exit condition: success or failure.
 */
int main(int argc, char* argv[])
{
    /*Handle invalid usage*/
    if (argc != 5)
    {
        fprintf(stderr, "Usage: ./car {name} {lowest floor} {highest floor} {delay}");
        return EXIT_FAILURE;
    }

    /*Declare variables*/
    car_shmem_ctrl* car; // Shared memory control struct
    char* name; // Name of elevator car (e.g. "A", "Service", "Test")
    /**
     * Range of accessible floors
	 * Must be exactly 3 characters + '\0' ("B99"<->"999")
	 */
    char floor_min[4];
    char floor_max[4];
    pthread_t tcp_thread;
    pthread_t internal_thread;

    /*Get required input arguments*/
    strcpy(name, argv[1]);
    strcpy(floor_min, argv[2]);
    strcpy(floor_max, argv[3]);
    delay = atoi(argv[4]);

    /*Initialise car shared memory*/
    car = malloc(sizeof(car_shmem_ctrl)); // Memory must be allocated for this shared object
    car_init(car, name);

    /*Initialise car data from input args*/
    strcpy(car->data->current_floor, floor_min);
    strcpy(car->data->destination_floor, floor_min);
    strcpy(car->data->status, str_closed);

    /*Create threads*/
    if (pthread_create(&tcp_thread, NULL, connect_controller, car) != 0)
    {
        fprintf(stderr, "\nUnable to create car TCP thread.\n");
        return EXIT_FAILURE;
    }
    if (pthread_create(&internal_thread, NULL, car_run, car) != 0)
    {
        fprintf(stderr, "\nUnable to create car internal thread.\n");
        return EXIT_FAILURE;
    }

    /*Terminate threads*/
    pthread_join(tcp_thread, NULL);
    pthread_join(internal_thread, NULL);

    /*Terminate shared mem structure*/
    car_kill(car);

    return EXIT_SUCCESS;
}

/**
 * Creates shared memory structure & initialises modes.
 * @param car Shared mem control struct for a car; contains car data struct.
 * @param name String to be appended to the designation "car" to form shared mem object name.
 */
void car_init(car_shmem_ctrl* car, char* name)
{
    /*Assemble car name*/
    strcpy(car->name, str_car);
    strcat(car->name, name);

    /*Remove any previous instances*/
    shm_unlink(car->name);

    /**
     * Create a new instance of this shared memory object.
     * Macros copied from tutorial exercises.
     */
    car->fd = shm_open(car->name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (car->fd == -1)
    {
        car->data = NULL;
        fprintf(stderr, "\nUnable to create shared memory object.\n");
        return;
    }
    if (ftruncate(car->fd, shmem_size) == -1)
    {
        car->data = NULL;
        fprintf(stderr, "\nUnable to set capacity of shared memory object.\n");
        return;
    }
    car->data = mmap(NULL, shmem_size, PROT_READ | PROT_WRITE, MAP_SHARED, car->fd, 0);
    if (car->data == MAP_FAILED)
    {
        fprintf(stderr, "\nUnable to map shared memory.\n");
        return;
    }

    /*Configure mutex lock*/
    pthread_mutexattr_t mutex_attr; // Attribute object needed to initialise the mutex
    pthread_mutexattr_init(&mutex_attr); // Initialise mutex attribute
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED); // Enable pshared

    pthread_mutex_init(&car->data->mutex, &mutex_attr); // Initialise mutex with pshared enabled

    pthread_mutexattr_destroy(&mutex_attr);

    /*Configure condition*/
    pthread_condattr_t cond_attr; // Attribute object needed to initialise the condition
    pthread_condattr_init(&cond_attr); // Initialise condition attribute
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED); // Enable pshared

    pthread_cond_init(&car->data->cond, &cond_attr); // Initialise cond with pshared enabled

    pthread_condattr_destroy(&cond_attr);

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

/**
 * Deletes shared memory structure.
 * @param car Shared mem control structure to be deleted.
 * PRE: `car` has been created using `car_init()`.
 */
void car_kill(car_shmem_ctrl* car)
{
    pthread_mutex_destroy(&car->data->mutex);
    munmap(car->data, shmem_size);
    shm_unlink(car->name);
    car->fd = -1;
    car->data = NULL;
}

/**
 * Opens TCP connection to controller.
 * @param ptr Car to be registered by the controller.
 */
void* connect_controller(void* ptr)
{
    const car_shmem_ctrl* car = ptr;
    int fd = -1;
    struct sockaddr_in server_addr;

    /*Initialise server data structure*/
    memset(&server_addr, '\0', SOCKLEN);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    /*Establish connection*/
    while (1)
    {
        if (car->data->safety_system) // Only connect if safety system is operating
        {
            fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd != -1
                && bind(fd, (struct sockaddr*)&server_addr, SOCKLEN) != -1)
            {
                break; // Connection successful; move on to handle comms
            }
        }
        fprintf(stderr, "\n%s failed to connect to controller server. Retrying in %d milliseconds...\n",
            car->name, delay);
        sleep(delay / 1000); // Convert ms -> s
    }

    /*Send registration message to controller*/

    signal(SIGPIPE, SIG_IGN); // Don't know what this does

    return NULL;
}

/**
 * Resets modifications to shared mem structure by internal controls.
 * @param ptr Shared mem control structure to be modified.
 */
void* car_run(void* ptr)
{
    const car_shmem_ctrl* car = ptr;

    pthread_mutex_lock(&car->data->mutex); // Lock mem struct

    /*Sleep this thread & unlock mutex until a button is pressed*/
    while (!car->data->open_button && !car->data->close_button)
    {
        pthread_cond_wait(&car->data->cond, &car->data->mutex);
    }

    if (car->data->open_button)
    {
        car->data->open_button = 0;
        pthread_cond_signal(&car->data->cond); // Signal contents changed
    }
    if (car->data->close_button)
    {
        car->data->close_button = 0;
        pthread_cond_signal(&car->data->cond); // Signal contents changed
    }

    pthread_mutex_unlock(&car->data->mutex); // Unlock mem struct

    return NULL;
}
