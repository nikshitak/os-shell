#include "libc.h"

int main(int argc, char** argv) {
    for(int i = 1; i < argc; i++){
        int fd = open(argv[i], 0x0);
        if(fd == -1){
            printf("cat: %s: No such file or directory\n", argv[i]);
        }else{
            cp(fd, 1);
            printf("\n");
        }
    }
}