#include "libc.h"

int main(int argc, char** argv) {
    
    int fd[2];  // File descriptors for the pipe

    int pipe_id = pipe(fd);

    if (pipe_id == -1) {
        printf("Pipe creation failed\n");
    }else{
        // Fork a child process
        printf("Make it into else statement\n");
        int pid = fork();

        if (pid == -1) {
            printf("Fork failed\n");
        }

        if (pid == 0) {
            // Child process
            printf("Make it into child process\n");
            close(fd[0]);  // Close the read end of the pipe
            char message[] = "Hello from the child process!\n";
            write(fd[1], message, sizeof(message));
            close(fd[1]);  // Close the write end of the pipe in the child
            printf("make it to end of child\n");
        } else {
            // Parent process
            uint32_t status = 42;
            wait(pid, &status);
            if (status != 0) {
                printf("wait failed\n");
            }
            printf("make it into parent process\n");
            // close(fd[1]);  // Close the write end of the pipe
            char buffer[100];
            read(fd[0], buffer, sizeof(buffer));
            printf("Parent received: %s", buffer);
            close(fd[0]);  // Close the read end of the pipe in the parent
        }
    }

    

    return 0;

}