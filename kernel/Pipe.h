#include "stdint.h"
#include "semaphore.h"
#include "debug.h"

class Pipe {
private:
    static const int MAX_BUFFER_SIZE = 1024; //Can adjust as needed? 
    char buffer[MAX_BUFFER_SIZE];
    int read_index; //index to "read" from (take bytes out of the buffer)
    int write_index; //index to "write" from (add bytes to the buffer)
    int count; //Count how many bytes are in the buffer
    Semaphore* read_sem; 
    Semaphore* write_sem; 
    Semaphore* lock;
public:

    Pipe(){
        read_index = 0;
        write_index = 0;
        count = 0;
        read_sem = new Semaphore(0);
        write_sem = new Semaphore(MAX_BUFFER_SIZE);
        lock = new Semaphore(1);
    }

    ~Pipe(){
        delete read_sem;
        delete write_sem;
        delete lock;
    }

    /* write */
    /* data is the array of bytes we wish to add to the buffer */
    /* size is the number of bytes that are in the data buffer */
    /* returns the number of bytes written */
    int write(const char* data, int size){
        lock->down(); //lock this operation
        int bytes_written = 0;
        for(int i = 0; i < size && count < MAX_BUFFER_SIZE; i++){
            write_sem->down();
            buffer[write_index] = data[i];
            count++;
            write_index = (write_index + 1) % MAX_BUFFER_SIZE; //Update index and makes buffer circular
            bytes_written++;
            read_sem->up();
        }
        Debug::printf("This is the pipes buffer: %s\n", buffer);
        lock->up();
        return bytes_written;
    }

    /* read */
    /* data is the buffer that we are putting the bytes into */
    /* size is the number of bytes that we are reading */
    /* returns the number of bytes read */
    int read(char* data, int size){
        lock->down();
        // Debug::printf("Are we reading?\n");
        int bytes_read = 0; 
        for(int i = 0; i < size && count > 0; i++){
            read_sem->down();
            data[i] = buffer[read_index];
            count--;
            read_index = (read_index + 1) % MAX_BUFFER_SIZE;
            bytes_read++;
            write_sem->up();
        }
        // Debug::printf("Here is what we are returning in pipe read: %s\n", data);
        lock->up();
        return bytes_read;
    }
};
