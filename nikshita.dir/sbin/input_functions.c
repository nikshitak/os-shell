#include "input_functions.h"
#include "libc.h"

int my_strncmp(char *s1, char *s2, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (s1[i] != s2[i]) {
            return 0;
        }
    }
    return 1;
}

void handle_tab(char *buf, int buffer_index) {
    char *path = ".";

    int fd = open(path, 0);
    if (fd < 0) {
        printf(
            "ls: cannot access '%s': No such file or "
            "directory\n",
            path);
    } else {
        char buffer[1024];
        int offset = 0;
        int rec_len = 0;
        int num_matches = 0;

        readdir(fd, buffer, offset);

        uint8_t name_len;
        char name[256];

        char **matches = malloc(255 * sizeof(char *));

        while (offset <= 1023) {
            offset += rec_len;
            uint16_t *rec_len_ptr =
                (uint16_t *)((uint8_t *)buffer + sizeof(uint32_t) + offset);
            uint8_t *name_len_ptr =
                (uint8_t *)((uint8_t *)buffer + sizeof(uint32_t) +
                            sizeof(uint16_t) + offset);
            char *name_ptr = (char *)((uint8_t *)buffer + sizeof(uint32_t) +
                                      sizeof(uint16_t) + sizeof(uint8_t) +
                                      sizeof(uint8_t) + offset);

            memcpy(&rec_len, rec_len_ptr, sizeof(uint16_t));
            memcpy(&name_len, name_len_ptr, sizeof(uint8_t));
            memcpy(name, name_ptr, name_len < 255 ? name_len + 1 : 255);
            name[name_len < 255 ? name_len : 254] =
                '\0';  // Ensure null termination

            if (my_strncmp(name, buf, buffer_index)) {
                matches[num_matches] = malloc((strlen(name)) + 1);
                memcpy(matches[num_matches], name, strlen(name));
                num_matches++;
            }
        }

        printf("\n");
        if (num_matches == 0) {
            printf("No matches found\n");
        } else if (num_matches == 1) {
            printf("%s\n", matches[0]);
        } else {
            for (int i = 0; i < num_matches; i++) {
                printf("%s\t", matches[i]);
            }
            printf("\n");
        }

        printf("$ %s", buf);

        for (int i = 0; i < num_matches; i++) {
            free(matches[i]);
        }
        free(matches);

    }
}

/*Read in user input with echo*/
int read_input_echo(char *buf, int buf_size) {
    int buffer_index = 0;
    char c;
    const char ENTER = 13;
    const char BACKSPACE = 127;
    const char TAB = 9;

    while (1) {
        read(0, &c, 1);  // Read one character from stdin
        if (c == ENTER) {
            buf[buffer_index] = '\0';
            break;
        } else if (c == BACKSPACE) {
            if (buffer_index !=
                0) {  // makes sure you don't delete the $ in terminal
                buffer_index -= 1;
                printf("\b \b");  // Erase the character by printing backspace,
                                  // space, and backspace again
            }
        } else if (c == TAB) {
            buf[buffer_index] = '\0';
            handle_tab(buf, buffer_index);
        } else {
            if (buffer_index <
                buf_size -
                    1) {  // Make sure buffer_index doesn't exceed buffer size
                buf[buffer_index] = c;
                printf("%c", c);  // Print the character
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
    const char TAB = 9;

    while (1) {
        read(0, &c, 1);  // Read one character from stdin
        if (c == ENTER) {
            buf[buffer_index] = '\0';
            break;
        } else if (c == BACKSPACE) {
            if (buffer_index !=
                0) {  // makes sure you don't delete the $ in terminal
                buffer_index -= 1;
            }
        } else if (c == TAB) {
            handle_tab(buf, buffer_index);
            printf("\nbuffer index: %d\n", buffer_index);
        } else {
            if (buffer_index <
                buf_size -
                    1) {  // Make sure buffer_index doesn't exceed buffer size
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

/*Source:: ChatGPT*/
char *strstr(const char *haystack, const char *needle) {
    if (*needle == '\0') {
        return (char *)haystack;
    }

    while (*haystack != '\0') {
        const char *h = haystack;
        const char *n = needle;

        while (*n != '\0' && *h == *n) {
            h++;
            n++;
        }

        if (*n == '\0') {
            return (char *)haystack;
        }

        haystack++;
    }

    return (void *)0;
}

/*Source:: ChatGPT*/
char *fgets(char *s, int size, int fd) {
    int bytesRead = 0;
    char c;
    while (bytesRead < size - 1 && read(fd, &c, 1) > 0) {
        if (c == '\n') {
            s[bytesRead++] = '\n';
            break;
        }
        s[bytesRead++] = c;
    }
    s[bytesRead] = '\0';
    if (bytesRead == 0) {
        return (void *)0;
    } else {
        return s;
    }
}

int strlen(const char *str) {
    long n = 0;
    while (*str++ != 0) n++;
    return n;
}