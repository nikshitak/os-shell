#include "libc.h"


int main(int argc, char **argv) {
    /*Init forks all of the processes that need to run, in this case login*/
    while(1){
        int id = fork();
        if(id == 0){
            execl("/sbin/login", 0);
            exit(0); // Exit child process after execvp
        }
        else if(id < 0){
            // Fork failed
            printf("Fork failed\n");
            exit(1);
        }
        else{
            // Parent process
            uint32_t status = 42;
            wait(id, &status); // Wait for child process to finish
            if (status != 0) {
                printf("wait failed 1\n");
            }
        }
    }
    shutdown();
    return 0;
}
