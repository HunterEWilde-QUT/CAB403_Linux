#include <regex.h>

#include "internal.h"
#include "status.h"

int check_valid_floors(car_shared_mem *);
int check_valid_status(char *);
int check_valid_states(car_shared_mem *);
int check_door_obstruction(car_shared_mem *);

int main(int argc, char *argv[]) {
    /*Handle invalid usage*/
    if (argc != 2) {
        fprintf(stderr, "Usage: ./safety {car name}");
        return EXIT_FAILURE;
    }

    /*Declare variables*/
    car_shmem_ctrl *car;

    /*Get name*/
    strcpy(car->name, str_car); // Initialise name of car object as "car"
    strcat(car->name, argv[1]); // Append car_name (e.g. "A") onto name of car object: "carA"

    /*Get shared memory structure from name*/
    car->fd = shm_open(car->name, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (car->fd == -1) {
        fprintf(stderr, "Unable to access car %s.", car->name);
        return EXIT_FAILURE;
    }
    car->data = mmap(NULL, sizeof(car_shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED, car->fd, 0);
    if (car->data == MAP_FAILED) {
        fprintf(stderr, "Unable to map shared memory segment for car %s.", car->name);
        return EXIT_FAILURE;
    }

    /*Handle conditions*/
    if (car->data->safety_system != 1) {
        car->data->safety_system = 1;
    }
    if (car->data->door_obstruction && car->data->status == str_closing) {
        /*Obstruction detected*/
        strcpy(car->data->status, str_closing);
    }
    if (car->data->emergency_stop && !car->data->emergency_mode) {
        fprintf(stderr, "The emergency stop button has been pressed!");
        car->data->emergency_mode = 1;
        car->data->emergency_stop = 0;
    }
    if (car->data->overload && !car->data->emergency_mode) {
        fprintf(stderr, "The overload sensor has been tripped!");
        car->data->emergency_mode = 1;
    }
    if (!car->data->emergency_mode && (check_valid_floors(car->data)
        || check_valid_status(car->data->status)
        || check_valid_states(car->data)
        || check_door_obstruction(car->data)))
    {
        fprintf(stderr, "Data consistency error!");
        car->data->emergency_mode = 1;
    }

    return EXIT_SUCCESS;
}

int check_valid_floors(car_shared_mem *data) {
    regex_t valid_floor; // Declare a regular expression.
    regcomp(&valid_floor, "^B?[1-9][0-9]*$", 0); // Compile regex for testing floor name validity.
    int valid = regexec(&valid_floor, car->data->current_floor, 0, NULL, 0) != 0
        || regexec(&valid_floor, car->data->destination_floor, 0, NULL, 0) != 0;
    regfree(&valid_floor); // Not needed hereafter.

    return valid;
}

int check_valid_status(char status[8]) {
    return status != str_open
        && status != str_opening
        && status != str_closed
        && status != str_closing
        && status != str_between;
}

int check_valid_states(car_shared_mem *data) {
    return data->open_button > 1
        || data->close_button > 1
        || data->safety_system > 1
        || data->door_obstruction > 1
        || data->overload > 1
        || data->emergency_stop > 1
        || data->individual_service_mode > 1;
}

int check_door_obstruction(car_shared_mem *data) {
    return data->door_obstruction && (data->status != str_opening
        || data->status != str_closing);
}
