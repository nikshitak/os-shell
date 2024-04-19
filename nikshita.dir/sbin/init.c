#include "libc.h"

int BACKSPACE = 127;
int TAB = 9;
int PIPE = 124;
int ENTER = 13;
int SPACE = 32;
int MAX_INPUT_LENGTH = 255;
char starting_path[256] = "/sbin/";

// char* shell_commands = {"ls", "cd", "echo", "pwd",  "cat",  "pipe",
//                          "touch", "mv", "cp", "find", "grep", "clear"};
int my_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int main(int argc, char **argv) {
    char buf[MAX_INPUT_LENGTH + 1];
    int buffer_index = 0;
    while (1) {
        // Get user input
        printf("$ ");
        buffer_index = 0;
        while (1) {
            char c;
            read(0, &c, 1);
            if (c == ENTER) {
                buf[buffer_index] = '\0';
                break;
            } else if (c == BACKSPACE) {
                if (buffer_index != 0) {  // makes sure you don't delete the $ in terminal
                    buffer_index -= 1;
                    printf("\b \b");
                }
            } else {
                buf[buffer_index] = c;
                printf("%c", c);
                buffer_index++;
            }
        }

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
            if (arg_index >= MAX_INPUT_LENGTH - 1) {
                break;  // Avoid overflow
            }
        }
        arguments[arg_index] = '\0';  // Set the last element to NULL for execvp

        printf("\n");  // prints output after user presses enter on a new line
        int id = fork();
        if (id == 0) {
            // int j = 0;
            // while (arguments[j]) {
            //     printf("arg %d: %s\n", j, arguments[j]);
            //     j++;
            // }
            /* child process */
            int size = sizeof(arguments[0]);
            memcpy(starting_path + 6, arguments[0], size);

            int stat = execvp(starting_path, arguments);
            if (stat == -1) {
                printf("invalid command: %s\n", arguments[0]);
            }

        } else if (id > 0) {
            /* parent process */
            uint32_t status = 42;
            wait(id, &status);
            if (status != 0) {
                printf("wait failed\n");
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
