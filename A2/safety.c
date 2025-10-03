#include "internal.h"
#include "status.h"

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

    return EXIT_SUCCESS;
}
