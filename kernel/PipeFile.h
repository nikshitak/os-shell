#include "atomic.h"
#include "file.h"
#include "libk.h"
#include "shared.h"
#include "stdint.h"
#include "debug.h"
#include "Pipe.h"

class PipeFile : public File {
    Pipe* my_pipe;
    int mode;
    const static int READ = 0;
    const static int WRITE = 1;

public:
    PipeFile(Pipe* pipe, int mode) : my_pipe(pipe), mode(mode){}

    bool isFile() override { return true; }
    bool isDirectory() override { return false; }
    off_t seek(off_t offset) { return -1; }
    off_t size() { return -1; }

    ssize_t read(void* buffer, size_t n) {
        if(mode != READ || my_pipe == nullptr){
            return -1;
        }
        return my_pipe->read((char*)buffer, n);
    }

    ssize_t write(void* buf, size_t size) {
        if(mode != WRITE || my_pipe == nullptr){
            return -1;
        }
        return my_pipe->write((char*)buf, size);
    }

    friend class Shared<PipeFile>;
    friend class Shared<File>;
};
