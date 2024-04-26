#include "libc.h"
#include "sys.h"

int main(int argc, char **argv) {
    char *path = "sbin/testfolder"; // should be the first argument in argv

    int fd = open(path, 0);
    if (fd < 0) {
        printf("ls: cannot access '%s': No such file or directory\n", path);
        return 1;
    }

    char buffer[1024];
    int offset = 0;
    int rec_len = 0;

    readdir(fd, buffer, offset);

    uint8_t name_len;
    char name[256];

    while (offset <= 1023) {
        offset += rec_len;
        uint16_t *rec_len_ptr = (uint16_t *)((uint8_t *)buffer + sizeof(uint32_t) + offset);
        uint8_t *name_len_ptr = (uint8_t *)((uint8_t *)buffer + sizeof(uint32_t) + sizeof(uint16_t) + offset);
        char *name_ptr = (char *)((uint8_t *)buffer + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) + offset);

        memcpy(&rec_len, rec_len_ptr, sizeof(uint16_t));
        memcpy(&name_len, name_len_ptr, sizeof(uint8_t));
        memcpy(name, name_ptr, name_len < 255 ? name_len + 1 : 255);
        name[name_len < 255 ? name_len : 254] = '\0';  // Ensure null termination

        if (name[0] != '.') {
            printf("%s\t", name);
        }
    }
    printf("\n");
    return 0;
}