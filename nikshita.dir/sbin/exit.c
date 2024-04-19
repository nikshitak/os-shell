#include "libc.h"

int main() {
    printf("Exiting the kernel.\n");
    shutdown();
}