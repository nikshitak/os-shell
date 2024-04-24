#include "libc.h"
#include "input_functions.h"

#define MAX_CHAR_LIMIT 20
#define MAX_USERS 4

struct User{
    char username[MAX_CHAR_LIMIT];
    char password[MAX_CHAR_LIMIT];
};

struct User users[MAX_USERS] = {
    {"bob", "bob"},
    {"urmom", "urmomdotcom"},
    {"test", "test123"}
};

int login(char *username, char *password) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (my_strcmp(users[i].username, username) == 0 && my_strcmp(users[i].password, password) == 0) {
            return 1; // Login successful
        }
    }
    return 0; // Login failed
}

int main(int argc, char **argv) {
    /*Perform login check, and then fork shell upon success*/
    char username[MAX_CHAR_LIMIT];
    char password[MAX_CHAR_LIMIT];
    printf("Username: ");
    while(1){
        read_input_echo(username, MAX_CHAR_LIMIT);
        printf("\nPassword: ");
        read_input_silent(password, MAX_CHAR_LIMIT);
        int login_status = login(username, password);
        if(login_status == 0){
            printf("login failed\n");
            printf("Username: ");
            continue;
        }
        printf("\nLogin successful! Hello %s\n", username);
        int id = fork();
        if(id == 0){
            execl("/sbin/our_shell", 0);
            exit(0); // Exit child process after execve
        }else if (id > 0) {
            uint32_t status = 42;
            wait(id, &status);
            if (status != 0) {
                printf("wait failed 2\n");
            }
        } else {
            printf("fork failed\n");
        }

    }
    shutdown();
    return 0;
}
