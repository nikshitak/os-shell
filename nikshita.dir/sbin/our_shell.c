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
        char *arguments[MAX_INPUT_LENGTH + 1];
        int arg_index = 0;
        int arg_start = 0;
        for (int i = 0; i <= buffer_index; i++) {
            if (buf[i] == SPACE || buf[i] == '\0') {
                buf[i] = '\0';  // Null-terminate the argument
                arguments[arg_index++] = &buf[arg_start];
                arg_start = i + 1;  // Set the start of the next argument
            }
            if (buf[i] == TAB) {
                printf("section 2\n");
                buf[buffer_index] = '\0';
                handle_tab(buf, buffer_index);
                buffer_index = 0;
                // continue;
            }

            if (arg_index >= MAX_INPUT_LENGTH - 1) {
                break;  // Avoid overflow
            }
        }
        arguments[arg_index] = '\0';  // Set the last element to NULL for execvp

        printf("\n");  // prints output after user presses enter on a new line

        int id = fork();
        if (id == 0) {
            /* child process */
            int size = strlen(arguments[0]);
            memcpy(starting_path + 6, arguments[0], size);
            // printf("path %s\n", starting_path);
            int stat = execvp(starting_path, arguments);
            if (stat == -1) {
                printf("invalid command: %s\n", arguments[0]);
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

        // TODO: hardcode exit to exit current shell
        // important when running nested shells.
    }

    shutdown();
    return 0;
}
