#include "libc.h"
#include "input_functions.h"

int main(int argc, char** argv) {
    //This is only until I am able to read directories with opendir and readdir
    // for(int i = 0; i < argc; i++){
    //     printf("Grep argv[%d]: %s\n", i, argv[i]);
    // }
    // if(argc != 3){
    //     printf("Usage: %s <filename> <pattern>\n", argv[0]);
    // }

    const char* pattern = argv[1];
    const char* filename = argv[2];

    int fd;
    if(filename != (void*)0){
        fd = open(filename, 0x0);
        if(fd == -1){
            printf("cat: %s: No such file or directory\n", filename);
        }
    }else{
        printf("this is null\n");
        fd = 0;
        pattern = "nice";
    }

    // printf("Make it to search fd: %d\n", fd);
    char line[1024]; // Adjust the size according to your needs
    while (fgets(line, sizeof(line), fd)) {
        // printf("this is my line: %s\n", line);
        if (strstr(line, pattern)) {
            printf("%s", line);
        }
    }


}