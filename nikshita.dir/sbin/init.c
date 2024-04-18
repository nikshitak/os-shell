#include "libc.h"

int BACKSPACE = 127;
int TAB = 9;
int PIPE = 124;
int ENTER = 13;
int SPACE = 32;
int MAX_INPUT_LENGTH = 255;

int main(int argc, char** argv) {
    char buf[MAX_INPUT_LENGTH + 1];
    int buffer_index = 0;
    printf("$ ");
    while(1){
        read(0, buf + buffer_index, 5);
        buffer_index++;
        if((int)buf[buffer_index - 1] == ENTER){
            buf[buffer_index + 1] = '\0';
            printf("\nabout to wrap it up... lets see if we hit a backspace in this string\n");
            for(int i = 0; i < buffer_index + 1; i++){
                char rv = buf[i];
                if((int)rv == BACKSPACE){
                    printf("THERE IS A BACKSPACE IN THIS STRING\n");
                }else{
                    printf("%c", rv);
                }
            }
            break;
        }
        if((int)buf[buffer_index - 1] == BACKSPACE){
            printf("Backspace");
        }
        printf("%c", buf[buffer_index - 1]);
    }
    printf("\nFinal String: %s\n", buf);
    // printf("We done!\n");

    /* this code parses through user input */
    char* tokens[256];
    char substring[256];

    buffer_index = 0;
    int substring_index = 0;
    int token_index = 0;

    while (buf[buffer_index] != '\0') {
        if (buf[buffer_index] == SPACE) {
            substring[substring_index] = '\0';
            tokens[token_index] = substring;
            token_index++;
            // printf("substring: %s\n", tokens[token_index - 1]);
            substring_index = 0;
            buffer_index++;
        } else {
            substring[substring_index] = buf[buffer_index];
            substring_index++;
            buffer_index++;
        }
    }

    // Handle the last substring (if any)
    if (substring_index > 0) {
        substring[substring_index] = '\0';
        tokens[token_index] = substring;
        token_index++;
        // printf("last substring: %s\n", tokens[token_index - 1]);
    }

    // all strings are in tokens. hopefully
    // TODO: put prompt in a while(1) loop, break... never. keep promptign user
    // if command isn't recognized, throw an error (have an array of strings for now)
    // call fork, see if inputs are being called in fork
    // modify fork so that instead of switching to user, it does... something else. run the shell call? see if it's valid? 
        // probably see if it's valid
    // see if you need to modify execl to deal with Shell calls now. 


    shutdown();
    return 0;
}
