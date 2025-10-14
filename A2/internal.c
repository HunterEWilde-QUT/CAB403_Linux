#include "internal.h"
#include "keywords.h"

int main(int argc, char* argv[])
{
    /*Handle invalid usage*/
    if (argc != 3)
    {
        fprintf(stderr, "\nUsage: ./internal {car name} {operation}\n");
        return EXIT_FAILURE;
    }

    /*Declare variables*/
    car_shmem_ctrl* car;
    char* operation;

    /*Get required input arguments*/
    strcpy(car->name, str_car); // Initialise name of car object as "car"
    strcat(car->name, argv[1]); // Append car_name (e.g. "A") onto name of car object: "carA"
    strcpy(operation, argv[2]);

    /*Get shared memory structure from name*/
    car->fd = shm_open(car->name, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (car->fd == -1)
    {
        fprintf(stderr, "\nUnable to access car %s.\n", car->name);
        return EXIT_FAILURE;
    }
    car->data = mmap(NULL, sizeof(car_shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED, car->fd, 0);
    if (car->data == MAP_FAILED)
    {
        fprintf(stderr, "\nUnable to map shared memory segment for car %s.\n", car->name);
        return EXIT_FAILURE;
    }

    /*Handle operations*/
    if (operation == "open")
    {
        car->data->open_button = 1;
    }
    else if (operation == "close")
    {
        car->data->close_button = 1;
    }
    else if (operation == "stop")
    {
        car->data->emergency_stop = 1;
    }
    else if (operation == "service_on")
    {
        car->data->individual_service_mode = 1;
        car->data->emergency_mode = 0;
    }
    else if (operation == "service_off")
    {
        car->data->individual_service_mode = 0;
    }
    else if (operation == "up" || operation == "down")
    {
        /*Check prerequisites to manual floor traversal*/
        if (car->data->individual_service_mode != 1)
        {
            fprintf(stderr, "\nOperation only allowed in service mode.\n");
            return EXIT_FAILURE;
        }
        if (car->data->status == str_open || car->data->status == str_opening || car->data->status == str_closing)
        {
            fprintf(stderr, "\nOperation not allowed while doors are open.\n");
            return EXIT_FAILURE;
        }
        if (car->data->status == str_between)
        {
            fprintf(stderr, "\nOperation not allowed while elevator is moving.\n");
            return EXIT_FAILURE;
        }

        uint8_t next_lvl;   // Next integer **level**.
        uint8_t B;          // Presence of 'B' prefix denoting basement level.

        /*Check for non-integer floor name (i.e. a basement floor)*/
        if (car->data->current_floor[0] == 'B')
        {
            /*B99 -> B1*/
            B = 1;
            next_lvl = atoi(&car->data->current_floor[1]); // Get numbers after 'B'.
        }
        else
        {
            /*1 -> 999*/
            B = 0;
            next_lvl = atoi(car->data->current_floor);
        }

        if (operation == "up")
        {
            if (B)
            {
                /*B99 -> B1*/
                next_lvl--; // Rise by one floor.
            }
            else
            {
                /*1 -> 999*/
                next_lvl++; // Rise by one floor.
            }

            /**
             * Check if next floor is a possible position:
             * If the next level is 0, the car has risen from floor B1 to floor 1.
             * If the next level is 1000, the car has reached the highest floor of 999.
             */
            if (next_lvl == 0)
            {
                strcpy(car->data->destination_floor, "1");
            }
            else if (next_lvl > 999)
            {
                fprintf(stderr, "\nCar cannot rise above highest floor.\n");
                return EXIT_FAILURE;
            }
        }
        else if (operation == "down")
        {
            if (B)
            {
                /*B99 -> B1*/
                next_lvl++; // Fall by one floor.
            }
            else
            {
                /*1 -> 999*/
                next_lvl--; // Fall by one floor.
            }

            /**
             * Check if next floor is a possible position:
             * If the next level is 0, the car has fallen from floor 1 to floor B1.
             * If the next leve is 100, the car has reached the lowest floor of B99.
             */
            if (next_lvl == 0)
            {
                strcpy(car->data->destination_floor, "B1");
            }
            else if (next_lvl > 99)
            {
                fprintf(stderr, "\nCar cannot fall below lowest floor.\n");
                return EXIT_FAILURE;
            }
        }

        /*Write to the car's destination floor*/
        char next_floor[4];
        sprintf(next_floor, "%u", next_lvl); // Convert unsigned int to string.

        if (B)
        {
            strcpy(car->data->destination_floor, "B");
            strcat(car->data->destination_floor, next_floor);
        }
        else
        {
            strcpy(car->data->destination_floor, next_floor);
        }
    }
    else
    {
        fprintf(stderr, "\nInvalid operation.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
