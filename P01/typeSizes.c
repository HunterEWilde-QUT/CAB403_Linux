#include <stdio.h>

int main() {
    char *char_ptr;
    int *int_ptr;
    float *flt_ptr;
    double *double_ptr;

    printf("char    %zu b\n", sizeof(char));
    printf("int     %zu b\n", sizeof(int));
    printf("float   %zu b\n", sizeof(float));
    printf("double  %zu b\n", sizeof(double));
}
