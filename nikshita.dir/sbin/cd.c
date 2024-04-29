#include "sys.h"
#include "libc.h"

// #include "../os-shell/kernel/process.h"

#include "threads.h"//can delete 


int main(int argc, char** argv){
    
    char* potential_path = argv[1];  

    //call cd on new path 
    cd(potential_path, 0); 

    //check if new file is valid 
    
    if (open(potential_path, 0) == -1){
        //if not valid, throw error 
        printf("cd: %s: No such file or directory\n", potential_path);
    }
    else {
        //if valid, set the new path in cd 
        cd(potential_path, 1); 
    }

    // printf("cd = %s\n", potential_path); 
    //done 
    return 0; 

}


