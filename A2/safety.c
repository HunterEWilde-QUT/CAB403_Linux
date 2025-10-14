#include <regex.h>
#include "internal.h"
#include "keywords.h"

uint8_t check_data_inconsistency(car_shared_mem*);

/*Use pointers for multithreading*/
void* safety_ctrl(void*);

/**
 * Handles failsafe conditions for cars.
 * @param argc Number of command line arguments: 1.
 * @param argv car_name.
 * @return Exit condition: success or failure.
 */
int main(int argc, char* argv[])
{
    /*Handle invalid usage*/
    if (argc != 2)
    {
        fprintf(stderr, "\nUsage: ./safety {car name}\n");
        return EXIT_FAILURE;
    }

    /*Declare variables*/
    car_shmem_ctrl* car;
    pthread_t thread;

    /*Get name*/
    strcpy(car->name, str_car); // Initialise name of car object as "car"
    strcat(car->name, argv[1]); // Append car_name (e.g. "A") onto name of car object: "carA"

    /**
     * Get shared memory structure from name.
     * Macros copied from tutorial exercises.
     */
    car->fd = shm_open(car->name, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (car->fd == -1)
    {
        fprintf(stderr, "Unable to access car %s.\n", car->name);
        return EXIT_FAILURE;
    }
    car->data = mmap(NULL, sizeof(car_shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED, car->fd, 0);
    if (car->data == MAP_FAILED)
    {
        fprintf(stderr, "\nUnable to map shared memory segment for car %s.\n", car->name);
        return EXIT_FAILURE;
    }

    /*Start safety control thread*/
    if (pthread_create(&thread, NULL, safety_ctrl, car) != 0)
    {
        fprintf(stderr, "\nUnable to create safety thread for car %s.\n", car->name);
        return EXIT_FAILURE;
    }

    /*Terminate thread*/
    pthread_join(thread, NULL);

    return EXIT_SUCCESS;
}

/**
 * Checks for any data inconsistency in the shared memory structure of a car.
 * @param data Shared mem data structure to be checked.
 * @return True if there is at least one data inconsistency, false if there are none.
 */
uint8_t check_data_inconsistency(car_shared_mem* data)
{
    /*Valid floors*/
    regex_t valid_floor; // Declare a regular expression
    regcomp(&valid_floor, "^B?[1-9][0-9]*$", 0); // Compile regex for testing floor name validity

    uint8_t invalid_floors = regexec(&valid_floor, data->current_floor, 0, NULL, 0) != 0
        || regexec(&valid_floor, data->destination_floor, 0, NULL, 0) != 0;

    regfree(&valid_floor); // Not needed hereafter

    /*Valid status*/
    uint8_t invalid_status = data->status != str_open
        && data->status != str_opening
        && data->status != str_closed
        && data->status != str_closing
        && data->status != str_between;

    /*Valid states*/
    uint8_t invalid_states = data->open_button > 1
        || data->close_button > 1
        || data->safety_system > 1
        || data->door_obstruction > 1
        || data->overload > 1
        || data->emergency_stop > 1
        || data->individual_service_mode > 1;

    /*Door obstruction*/
    uint8_t door_obstruction = data->door_obstruction
        && (data->status != str_opening || data->status != str_closing);

    return invalid_floors || invalid_status || invalid_states || door_obstruction;
}

/**
 * Handle failsafe conditions.
 * @param ptr Shared mem control structure to be checked & modified.
 */
void* safety_ctrl(void* ptr)
{
    const car_shmem_ctrl* car = ptr;

    pthread_mutex_lock(&car->data->mutex); // Lock mem struct

    /*Sleep this thread & unlock mutex until safety system is triggered*/
    while (car->data->safety_system
        && !car->data->door_obstruction
        && !car->data->emergency_stop
        && !car->data->overload
        && !check_data_inconsistency(car->data))
    {
        pthread_cond_wait(&car->data->cond, &car->data->mutex);
    }

    if (car->data->safety_system != 1)
    {
        car->data->safety_system = 1;

        pthread_cond_signal(&car->data->cond); // Signal contents changed
    }
    if (car->data->door_obstruction && car->data->status == str_closing)
    {
        /*Obstruction detected*/
        strcpy(car->data->status, str_opening);

        pthread_cond_signal(&car->data->cond); // Signal contents changed
    }
    if (car->data->emergency_stop && !car->data->emergency_mode)
    {
        fprintf(stderr, "\nThe emergency stop button has been pressed!\n");

        car->data->emergency_mode = 1;
        car->data->emergency_stop = 0;

        pthread_cond_signal(&car->data->cond); // Signal contents changed
    }
    if (car->data->overload && !car->data->emergency_mode)
    {
        fprintf(stderr, "\nThe overload sensor has been tripped!\n");

        car->data->emergency_mode = 1;

        pthread_cond_signal(&car->data->cond); // Signal contents changed
    }
    if (!car->data->emergency_mode && !check_data_inconsistency(car->data))
    {
        fprintf(stderr, "\nData consistency error!\n");

        car->data->emergency_mode = 1;

        pthread_cond_signal(&car->data->cond); // Signal contents changed
    }

    pthread_mutex_unlock(&car->data->mutex); // Unlock mem struct

    return NULL;
}
