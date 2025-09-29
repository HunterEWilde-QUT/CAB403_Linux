#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

/**
 * Summons a car to the specified source floor, & sets its destination, via the controller.
 */
int main(int argc, char *argv[]) {
    /*Handle invalid usage*/
    if (argc != 3) {
        fprintf(stderr, "Usage: ./call {source floor} {destination floor}");
        return EXIT_FAILURE;
    }

    /*Handle floor name exceptions*/
    if (argv[1] == argv[2]) {
        fprintf(stderr, "You are already on that floor!");
        return EXIT_FAILURE;
    }

    regex_t valid_floor; // Declare a regular expression.
    regcomp(&valid_floor, "^B?[1-9][0-9]*$", 0); // Compile regex for testing floor name validity.
    if (strlen(argv[1]) > 3 || regexec(&valid_floor, argv[1], 0, NULL, 0) != 0 || strlen(argv[2]) > 3 || regexec(&valid_floor, argv[2], 0, NULL, 0) != 0) {
        fprintf(stderr, "Invalid floor(s) specified.");
        return EXIT_FAILURE;
    }
    regfree(&valid_floor); // Not needed hereafter.

    /**
     * Declare variables
     * Floor names are always 1 to 3 characters.
     */
    char floor_src[4];
    char floor_dst[4];

    /*Get required input arguments*/
    strcpy(floor_src, argv[1]);
    strcpy(floor_dst, argv[2]);

    return EXIT_SUCCESS;
}
