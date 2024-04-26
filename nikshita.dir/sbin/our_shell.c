#include "input_functions.h"
#include "libc.h"

int BACKSPACE = 127;
int TAB = 9;
int PIPE = 124;
int ENTER = 13;
int SPACE = 32;
int MAX_INPUT_LENGTH = 255;
char starting_path[256] = "/sbin/";


int main(int argc, char **argv) {
    char buf[MAX_INPUT_LENGTH + 1];
    int buffer_index = 0;
    while (1) {
        // Get user input
        printf("$ ");
        buffer_index = read_input_echo(buf, MAX_INPUT_LENGTH + 1);
        // Parse buffer so that we can use it in execvp
        char *arguments[10][MAX_INPUT_LENGTH + 1];
        int command_num = 0;
        int arg_index = 0;
        int arg_start = 0;
        for (int i = 0; i <= buffer_index; i++) {
            if (buf[i] == SPACE || buf[i] == '\0') {
                buf[i] = '\0';  // Null-terminate the argument
                arguments[command_num][arg_index] = &buf[arg_start];
                arg_index++;
                arg_start = i + 1;  // Set the start of the next argument
            }
            if(buf[i] == PIPE){
                arguments[command_num][arg_index] = '\0';
                command_num++;
                arg_index = 0;
                arg_start += 2;
                i++;
            }
            if (arg_index >= MAX_INPUT_LENGTH - 1) {
                break;  // Avoid overflow
            }
        }
        arguments[command_num][arg_index] = '\0';  // Set the last element to NULL for execvp

        // for(int i = 0; i <= command_num; i++){
        //     for(int j = 0; arguments[i][j] != (int)'\0'; j++){
        //         // printf("HERE arguments[%d][%d]: %s\n", i, j, arguments[i][j]);
        //     }
        // }

        printf("\n");  // prints output after user presses enter on a new line

        if(command_num > 0){
            int pipefd[2];
            pipe(pipefd);
            int id = fork();
            if(id == 0){
                int size = sizeof(arguments[0][0]);
                memcpy(starting_path + 6, arguments[0][0], size);
                close(1);
                dup2(pipefd[1], 1);
                close(pipefd[1]);
                execvp(starting_path, arguments[0]);
            }else if(id > 0){
                uint32_t status = 42;
                wait(id, &status);
                if (status != 0) {
                    printf("wait failed 3\n");
                }
                int id2 = fork();
                if(id2 == 0){
                    int size = sizeof(arguments[1][0]);
                    memcpy(starting_path + 6, arguments[1][0], size);
                    close(0);
                    dup2(pipefd[0], 0);
                    close(pipefd[0]);
                    // int j = 0;
                    // while (arguments[1][j]) {
                    //     printf("arguments[1][%d] = %s\n", j, arguments[1][j]);
                    //     j++;
                    // }
                    execvp(starting_path, arguments[1]);
                }else if(id2 > 0){
                    uint32_t status2 = 42;
                    wait(id2, &status2); // <--- Corrected: waited for id2, not id
                    if (status2 != 0) { // <--- Corrected: checked status2, not status
                        printf("wait failed 3\n");
                    }
                }
            }
        }else{
            int id = fork();
            if (id == 0) {
                /* child process */
                int size = strlen(arguments[0][0]);
                memcpy(starting_path + 6, arguments[0][0], size);
                // printf("path %s\n", starting_path);
                int stat = execvp(starting_path, arguments[0]);
                if (stat == -1) {
                    printf("invalid command: %s\n", arguments[0][0]);
                }

            } else if (id > 0) {
                /* parent process */
                uint32_t status = 42;
                wait(id, &status);
                if (status != 0) {
                    printf("wait failed 3\n");
                }

            } else {
                printf("fork failed\n");
            }
        }

    }

    shutdown();
    return 0;
}
