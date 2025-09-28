#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "internal.h"

car_shared_mem car;

int main(int argc, char *argv[]) {
    /*Get required input arguments*/
    if (argc != 5) {
        fprintf(stderr, "Usage: ./car {name} {lowest floor} {highest floor} {delay}");
        return EXIT_FAILURE;
    }
    char name = atoi(argv[1]);
    char floor_min = atoi(argv[2]);
    char floor_max = atoi(argv[3]);
    int delay = atoi(argv[4]);

    /*Initialise car shared memory*/
    car_init(&name);

    return EXIT_SUCCESS;
}

void car_init(const char *name) {
    /*Assemble car name*/
    char *object = "car";
    strcat(object, name);

    /*Remove any previous instances & create a new instance of this shared memory object*/
    shm_unlink(object);
    int fd = shm_open(object, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    ftruncate(fd, sizeof(car_shared_mem));
}
