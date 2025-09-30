#include "internal.h"

int main(int argc, char *argv[]) {
    /*Handle invalid usage*/
    if (argc != 3) {
        fprintf(stderr, "Usage: ./internal {car name} {operation}");
        return EXIT_FAILURE;
    }

    /*Declare variables*/
    char *car_name;
    char *operation;
    car_shmem_ctrl *car;

    /*Get required input arguments*/
    strcpy(car_name, argv[1]);
    strcpy(operation, argv[2]);

    /*Get shared memory structure from name*/
    car->fd = shm_open(car_name, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (car->fd == -1) {
        fprintf(stderr, "Unable to open shared memory segment.");
        return EXIT_FAILURE;
    }
    car->data = mmap(NULL, sizeof(car_shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED, car->fd, 0);
    if (car->data == MAP_FAILED) {
        fprintf(stderr, "Unable to map shared memory segment.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
