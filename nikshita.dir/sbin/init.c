#include "libc.h"
/*
    This test case mimics t0 in that it doesn't test all of the
    system calls, just open, fork, read, write, execl, len, 
    close, and seek. 
*/

/*
    makes sure you're getting the right file directory and getting length right
    tests wrapper class since I struggled with that

    cp tests read and write specifically
*/
void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

/*
Opens and tests the other files using the syscalls mentioned above. 
*/
int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }
    int fd = open("/etc/data.txt",0);
    one(fd);

    printf("*** close = %d\n",close(fd));

    one(fd);
    one(100);


    printf("*** open again %d\n",open("/etc/data.txt",0));
    printf("*** seek %ld\n",seek(3,17));
    
    int id = fork();

    if (id < 0) {
        printf("fork failed");
    } else if (id == 0) {
        /* child */
        printf("*** in child\n");
        int rc = execl("/sbin/shell","shell","a","b","c",0);
        printf("*** execl failed, rc = %d\n",rc);
    } else {
        /* parent */
        uint32_t status = 42;
        wait(id,&status);
        printf("*** back from wait %ld\n",status);

        int fd = open("/etc/panic.txt",0);
        cp(fd,1);
    }

    shutdown();
    return 0;
}
