#include "internal.h"

int main(int argc, char *argv[]) {
    /*Handle invalid usage*/
    if (argc != 2) {
        fprintf(stderr, "Usage: ./safety {car name}");
        return EXIT_FAILURE;
    }

    /*Declare variables*/
    char *car_name;

    /*Get required input arguments*/
    strcpy(car_name, argv[1]);

    return EXIT_SUCCESS;
}
