#include "internal.h"

int main(int argc, char *argv[]) {
    /*Get required input arguments*/
    if (argc != 3) {
        fprintf(stderr, "Usage: ./internal {car name} {operation}");
        return EXIT_FAILURE;
    }
    char car_name = atoi(argv[1]);
    char operation = atoi(argv[2]);

    return EXIT_SUCCESS;
}
