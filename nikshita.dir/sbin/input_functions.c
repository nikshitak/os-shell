#include "libc.h"
#include "input_functions.h"


/*Read in user input with echo*/
int read_input_echo(char *buf, int buf_size) {
    int buffer_index = 0;
    char c;
    const char ENTER = 13;
    const char BACKSPACE = 127;

    while (1) {
        read(0, &c, 1); // Read one character from stdin
        if (c == ENTER) {
            buf[buffer_index] = '\0';
            break;
        } else if (c == BACKSPACE) {
            if (buffer_index != 0) {  // makes sure you don't delete the $ in terminal
                buffer_index -= 1;
                printf("\b \b"); // Erase the character by printing backspace, space, and backspace again
            }
        } else {
            if (buffer_index < buf_size - 1) { // Make sure buffer_index doesn't exceed buffer size
                buf[buffer_index] = c;
                printf("%c", c); // Print the character
                buffer_index++;
            }
        }
    }
    return buffer_index;
}

/*Read in user input WITHOUT echo*/
int read_input_silent(char *buf, int buf_size) {
    int buffer_index = 0;
    char c;
    const char ENTER = 13;
    const char BACKSPACE = 127;

    while (1) {
        read(0, &c, 1); // Read one character from stdin
        if (c == ENTER) {
            buf[buffer_index] = '\0';
            break;
        } else if (c == BACKSPACE) {
            if (buffer_index != 0) {  // makes sure you don't delete the $ in terminal
                buffer_index -= 1;
            }
        } else {
            if (buffer_index < buf_size - 1) { // Make sure buffer_index doesn't exceed buffer size
                buf[buffer_index] = c;
                buffer_index++;
            }
        }
    }
    return buffer_index;
}

/*Check if two strings are equal*/
int my_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}