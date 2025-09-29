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

    /*Get required input arguments*/
    strcpy(car_name, atoi(argv[1]));
    strcpy(operation, atoi(argv[2]));

    return EXIT_SUCCESS;
}
