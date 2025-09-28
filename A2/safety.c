#include "internal.h"

int main(int argc, char *argv[]) {
    /*Get required input arguments*/
    if (argc != 2) {
        fprintf(stderr, "Usage: ./safety {car name}");
        return EXIT_FAILURE;
    }
    char car_name = atoi(argv[1]);

    return EXIT_SUCCESS;
}
