#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    /**
     * Declare variables
     * Floor names are always 1 to 3 characters.
     */
    char floor_src[4];
    char floor_dst[4];

    /*Get required input arguments*/
    if (argc != 3) {
        fprintf(stderr, "Usage: ./call {source floor} {destination floor}");
        return EXIT_FAILURE;
    }
    strcpy(floor_src, atoi(argv[1]));
    strcpy(floor_dst, atoi(argv[2]));

    return EXIT_SUCCESS;
}
