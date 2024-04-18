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

    /* parse through the string */
    // int offset = 0;
    // buffer_index = 0;
    // int space_count = 0;

    // printf("after getting string\n");

    // // count # spaces in buffer
    // while (buf[buffer_index] != '\0') {
    //     printf("in here for how long\n");
    //     if ((int)buf[buffer_index] == SPACE) {
    //         space_count++;
    //     }
    //     buffer_index++;
    // }
    // printf("num spaces: %d\n", space_count);

    // // add tokens to tokens array
    // char* tokens[space_count + 1];
    // int token_index = 0;
    // tokens[space_count] = '\0'; // nullptr
    // buffer_index = 0;

    // while (buf[offset] != '\0' || buf[offset] != 0) {
    //     printf("offset: %d\n", offset);
    //     char token[256];
    //     while (buf[buffer_index] != SPACE) {
    //         token[buffer_index] = buf[buffer_index + offset];
    //         printf("letter: %c\n", token[buffer_index]);
    //         buffer_index++;
    //     }
    //     token[buffer_index + 1] = '\0';
    //     printf("token: %s\n", token);
    //     tokens[token_index] = token;
    //     offset += buffer_index + 1;
    //     buffer_index = 0;
    //     token_index++;
    // }

    // tokens[token_index] = 0;

    // token_index = 0;
    // while (tokens[token_index] != 0) {
    //     printf("token2: %s\n", tokens[token_index]);
    //     token_index++;
    // }

    char substring[256];

    buffer_index = 0;
    int substring_index = 0;

    while (buf[buffer_index] != '\0') {
        if (buf[buffer_index] == ' ') {
            substring[substring_index] = '\0';
            printf("substring: %s\n", substring);
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
        printf("last substring: %s\n", substring);
    }


    shutdown();
    return 0;
}
