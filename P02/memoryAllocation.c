#include <stdio.h>
#include <stdlib.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void enterValuesIntoArr();              // Permit the user to enter values which are stored in the array
void printOutArray();                   // Print out the values in an array
int askUserToIncreaseArraySize();       // Prompt user to enter the new increased size of the array
void enterValuesIntoExistingArray();    // Add values into an existing array that is partially populated

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Incorrect parameters - usage ./MemAlloc n\n");

        return EXIT_FAILURE;
    }
    
    int arrSize = atoi(argv[1]);
    int *arr = malloc(sizeof(int) * arrSize); // allocate memory

    enterValuesIntoArr(arr, arrSize);
    printOutArray(arr, arrSize);

    free(arr); // deallocate memory
    
    return EXIT_SUCCESS;
}

void enterValuesIntoArr(int *ptr, int arrSize) {
    for (int i = 0; i < arrSize; i++) {
        ptr[i] = i;
    }
}

void printOutArray(int *ptr, int arrSize) {
    for (int j = 0; j < arrSize; j++) {
        printf("arr[%d] = %d\n", j, ptr[j]);
    }
}
