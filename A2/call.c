#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int main(int argc, char *argv[]) {
    /*Handle invalid usage*/
    if (argc != 3) {
        fprintf(stderr, "Usage: ./call {source floor} {destination floor}");
        return EXIT_FAILURE;
    } else if (strlen(atoi(argv[1])) > 3 || strlen(atoi(argv[2])) > 3) {
        fprintf(stderr, "Invalid floor(s) specified.");
    }

    /**
     * Declare variables
     * Floor names are always 1 to 3 characters.
     */
    char floor_src[4];
    char floor_dst[4];

    /*Get required input arguments*/
    strcpy(floor_src, atoi(argv[1]));
    strcpy(floor_dst, atoi(argv[2]));

    /*Handle floor name exceptions*/
    if (floor_dst == floor_src) {
        printf("You are already on that floor!");
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
