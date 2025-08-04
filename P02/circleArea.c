#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define area(r) (M_PI * r * r)

int main(int argc, char *argv[]) {
    // printf("(pi=%f) ", M_PI);
    float r = atof(argv[1]);
    float A = area(r);
    printf("The area of a circle of radius %f is %f\n", r, A);

    return 0;
}
