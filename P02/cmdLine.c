#include <stdio.h>

#define arrayLength(array) (sizeof(array)/sizeof(array[0]))

int main(int argc, char *argv[]) {
    int arrayOne[] = {12, 15, 2, 19, 43, 22};
    int length = arrayLength(arrayOne);
    printf("The length of the array is %d\n", length);

    return 0;
}
