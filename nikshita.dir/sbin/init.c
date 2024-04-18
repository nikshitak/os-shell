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
    shutdown();
    return 0;
}
