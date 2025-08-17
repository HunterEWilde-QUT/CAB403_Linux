#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    for(int i = 1; i <= 5; i++) {
        pid_t pid = fork();
        if(pid == 0) {
            printf("Child => PPID: %d PID: %d\n", getppid(), getpid());
            exit(0);
        } else if(pid > 0) {
            printf("\nParent => PID: %d", getpid());
            printf("\nWaiting for child process to finish... ");
            wait(NULL);
            printf("Child process finished!\n");
        } else {
            printf("\nUnable to create child process.\n");
        }
    }
    return EXIT_SUCCESS;
}
