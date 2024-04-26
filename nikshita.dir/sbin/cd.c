#include "sys.h"
#include "libc.h"

// #include "../os-shell/kernel/process.h"

#include "threads.h"//can delete 


int main(int argc, char** argv){
    
    char* potential_path = argv[1];  
    

    // int stat = cd(potential_path); 

    cd(potential_path, 0); 
    // printf("potential path - %s\n", potential_path); 


    // printf("stat = %d\n", stat); 

    
    if (open(potential_path, 0) == -1){
        
        // printf("cd: %s: No such file or directory\n", potential_path);
    }
    else {
        cd(potential_path, 1); 
        // printf("cd = %s\n", potential_path); 
    }

    return 0; 

}


