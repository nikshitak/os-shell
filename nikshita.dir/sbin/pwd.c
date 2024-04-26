#include "sys.h"
#include "libc.h"

// #include "../os-shell/kernel/process.h"

// #include "threads.h"//can delete 

// #define MAX_CHARS 4096; //dummy number just for vibez 


int main(int argc, char** argv){

    char* path = (char*) ""; //overriden 

    // char* path = ;

    // if (open(path, 0) < 0){
    //     printf("pwd: cannot access '%s': No such file or directory\n", path);
    // }
    // else {
    //     printf("path is accessible - makes it here\n"); 
    // }

    


    // current()->process->set_cwd(); 

    // printf("%s\n", current()->process->get_cwd()); 

    // if (pwd(path) != 0){
    //     printf("pwd: cannot access '%s': No such file or directory\n", path);
    // } 

    // int stat = pwd(path);
    pwd(path);

    // pwd(path); 
    // printf("current pwd path - %s\n", path);

    // if (stat == -1){
        // printf("\npwd: cannot access '%s': No such file or directory\n", path);
        // printf("is this where it's going wrong or am i crazy\n"); 
    //     return 1; 
    // }
    // printf("pwd val here - %d\n", stat); 

    

    printf("%s\n", path); 

    return 0; 
    
}