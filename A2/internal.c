#include "internal.h"

#define CAR_MEM sizeof(car_shared_mem)

int main(int argc, char *argv[]) {
    /*Initialise car shared memory*/
    car_shared_mem carA = malloc(CAR_MEM);
    car_shared_mem carB = malloc(CAR_MEM);
    car_shared_mem carC = malloc(CAR_MEM);

    return EXIT_SUCCESS;
}
