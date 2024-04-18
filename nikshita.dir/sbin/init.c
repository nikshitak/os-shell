#include "libc.h"

int BACKSPACE = 127;
int TAB = 9;
int PIPE = 124;
int ENTER = 13;
int SPACE = 32;
int MAX_INPUT_LENGTH = 255;

// char* shell_commands = {"ls",    "cd", "echo", "pwd",  "cat",  "pipe",
//                          "touch", "mv", "cp",   "find", "grep", "clear"};
int my_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int main(int argc, char** argv) {
    char buf[MAX_INPUT_LENGTH + 1];
    int buffer_index = 0;
    while (1) {
        //Get user input
        printf("$ ");
        buffer_index = 0;
        while (1) {
            char c;
            read(0, &c, 1);
            if (c == ENTER) {
                buf[buffer_index] = '\0';
                break;
            } else if (c == BACKSPACE) {
                buffer_index -= 1;
                printf("\b \b");
            } else {
                buf[buffer_index] = c;
                printf("%c", c);
                buffer_index++;
            }
        }

        //Now that we have the buffer containing the user input, parse it so we can use it"
        char *arguments[MAX_INPUT_LENGTH + 1];
        int arg_index = 0;
        int arg_start = 0;
        for (int i = 0; i <= buffer_index; i++) {
            if(buf[i] == SPACE || buf[i] == '\0') {
                buf[i] = '\0'; // Null-terminate the argument
                arguments[arg_index++] = &buf[arg_start];
                arg_start = i + 1; // Set the start of the next argument
            }
            if(arg_index >= MAX_INPUT_LENGTH - 1) {
                break; // Avoid overflow
            }
        }
        arguments[arg_index] = '\0'; // Set the last element to NULL for execvp

        printf("\nCommand: %s\n", arguments[0]);

        // exit functionality 
        if(my_strcmp(arguments[0], "exit") == 0) {
            printf("Exiting the shell...\n");
            break;
        }

        //Now here is the echo functionality
        if(my_strcmp(arguments[0], "echo") == 0){
            int i = 1;
            while(arguments[i] != (void*)0){
                printf("%s ", arguments[i]);
                i++;
            }
            printf("\n"); // Print a newline at the end
        }





        // all strings are in tokens. hopefully
        // TODO: put prompt in a while(1) loop, break... never. keep promptign
        // user if command isn't recognized, throw an error (have an array of
        // strings for now) call fork, see if inputs are being called in fork
        // modify fork so that instead of switching to user, it does...
        // something else. run the shell call? see if it's valid? probably see
        // if it's valid see if you need to modify execl to deal with Shell
        // calls now.
    }
    shutdown();
    return 0;
}
