#include "libc.h"

/*This file is to have a location for us to put all of our user input functions*/


/* this function handles the functionality for the tab function */
void handle_tab(char *buf, int buffer_index);

/*This method will read in the user input from stdin with an echo updating on the terminal*/
/*Will return the index of the buffer that we stop at so it can be used for other operations*/
int read_input_echo(char *buf, int buf_size); 

/*This method will read in the user input from stdin WITHOUT an echo*/
/*Will return the index of the buffer that we stop at so it can be used for other operations*/
int read_input_silent(char *buf, int buf_size);

/*This is a quick implementation of strcmp so that we can use them in our implementation of commands*/
/*Return 0 if equal, num >= 1 otherwise*/
int my_strcmp(const char *s1, const char *s2);

/* compares strings via the first size number of characters */
int my_strncmp(char* s1, char* s2, int size);

char *strstr(const char *haystack, const char *needle);

char* fgets(char* s, int size, int fd);

/* get the length of the string until the \0 (nullptr) */
int strlen(const char* str);