#include <stdio.h>

int main() {
    int num = 1; // Placeholder number
    int sum = 0;
    do {
        printf("Current sum: %d\n", sum);
        printf("Enter number: ");
        // Read in an int from user input & write it to the address of `num`
        scanf("%d", &num);
        // Add user input to sum
        sum += num;
    } while(num != 0);
    printf("Final sum: %d\n", sum);
    return 0;
}