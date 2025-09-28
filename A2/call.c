#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    /*Get required input arguments*/
    if (argc != 3) {
        fprintf(stderr, "Usage: ./call {source floor} {destination floor}");
        return EXIT_FAILURE;
    }
    char floor_src = atoi(argv[1]);
    char floor_dst = atoi(argv[2]);

    return EXIT_SUCCESS;
}
