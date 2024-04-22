#include "libc.h"

/*This file is to have a location for us to put all of our user input functions*/

/*This method will read in the user input from stdin with an echo updating on the terminal*/
/*Will return the index of the buffer that we stop at so it can be used for other operations*/
int read_input_echo(char *buf, int buf_size); 

/*This method will read in the user input from stdin WITHOUT an echo*/
/*Will return the index of the buffer that we stop at so it can be used for other operations*/
int read_input_silent(char *buf, int buf_size);

/*This is a quick implementation of strcmp so that we can use them in our implementation of commands*/
/*Return 0 if equal, num >= 1 otherwise*/
int my_strcmp(const char *s1, const char *s2);