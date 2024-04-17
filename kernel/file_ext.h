#ifndef _file_ext_h_
#define _file_ext_h_

#include "atomic.h"
#include "file.h"
#include "libk.h"
#include "shared.h"
#include "stdint.h"

class ActualFile : public File {
    Shared<Node> node;
    uint32_t totalOffset;

public:
    ActualFile(Shared<Node> node) : node(node) { totalOffset = 0; }
    bool isFile() override { return node->is_file(); }
    bool isDirectory() override { return node->is_dir(); }
    off_t seek(off_t offset) {
        totalOffset = offset;
        return totalOffset;
    }

    off_t size() {
        return node->size_in_bytes();
    }

    ssize_t read(void* buffer, size_t n) {
        if (node == nullptr) {
            return -1;
        }

        uint32_t addr = (uint32_t) buffer;
        if (addr < 0x80000000) {
            return -1;
        }

        if (totalOffset >= size() || n == 0) {
            return 0;
        }

        uint32_t bytes_read = n;
        if (bytes_read > ((uint32_t) size()) - totalOffset) {
            bytes_read = size() - totalOffset;
        }

        node->read_all(totalOffset, bytes_read, (char*) buffer);
        totalOffset += bytes_read;
        return bytes_read;
    }

    ssize_t write(void* buf, size_t size) { return -1; }

    friend class Shared<ActualFile>;
    friend class Shared<File>;
};

#endif