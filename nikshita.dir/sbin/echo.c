#include "libc.h"

int main(int argc, char** argv) {
    int i = 1;
    printf("\n"); // Print a newline at the beginning
    while(argv[i] != (void*)0){
        printf("%s ", argv[i]);
        i++;
    }
    printf("\n"); // Print a newline at the end
    exit(0);
}