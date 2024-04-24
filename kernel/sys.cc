#include "sys.h"

#include "debug.h"
#include "elf.h"
#include "ext2.h"
#include "file.h"
#include "file_ext.h"
#include "heap.h"
#include "idt.h"
#include "kernel.h"
#include "libk.h"
#include "machine.h"
#include "process.h"
#include "shared.h"
#include "stdint.h"
#include "threads.h"

struct DirectoryEntry {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
};

int SYS::exec(const char* path, int argc, const char* argv[]) {
    using namespace gheith;
    ElfHeader elfheader;
    // Debug::printf("path: %s\n", path);

    // Debug::printf("do we get to sys::exec\n");

    auto file = root_fs->find(root_fs->root, path);

    if (file == nullptr) {
        return -1;
    }
    if (!file->is_file()) {
        Debug::printf("file is not a file\n");
        Debug::printf("file type: %d\n", file->get_type());
        return -2;
    }

    file->read(0, elfheader);

    // elf checks
    if (elfheader.maigc0 != 0x7F || elfheader.magic1 != 'E' ||
        elfheader.magic2 != 'L' || elfheader.magic3 != 'F') {
        Debug::printf("wrong magic numbers\n");
        return -3;
    } else if (elfheader.encoding != 1) {
        Debug::printf("encoding is wrong\n");
        return -4;
    } else if (elfheader.cls != 1) {
        Debug::printf("cls is wrong\n");
        return -5;
    } else if (elfheader.abi != 0) {
        Debug::printf("abi is wrong\n");
        return -6;
    } else if (elfheader.type != 2) {
        Debug::printf("type is wrong\n");
        return -7;
    } else if (elfheader.version != 1) {
        Debug::printf("version is wrong\n");
        return -8;
    } else if (elfheader.phoff == 0) {
        Debug::printf("phoff is wrong\n");
        return -9;
    }

    current()->process->clear_private();

    uint32_t sp = 0xefffe000;

    // add strings to stack
    int num = argc;
    uint32_t* addresses = new uint32_t[argc];

    while (num > 0) {
        int size = K::strlen(argv[num - 1]) + 1;
        size = size + (4 - (size % 4));
        char* elem = new char[size];
        memcpy(elem, argv[num - 1], size);
        sp -= size;
        addresses[num - 1] = sp;

        char* pointer = (char*)sp;
        memcpy(pointer, elem, size);
        num--;
    }

    sp -= 4;
    bzero((char*)sp, 4);

    for (int i = argc - 1; i >= 0; i--) {
        sp -= 4;
        uint32_t* argvP = (uint32_t*)sp;
        argvP[0] = addresses[i];
    }

    sp -= 4;
    uint32_t* argP = (uint32_t*)sp;
    argP[0] = (sp + 4);

    sp -= 4;
    argP = (uint32_t*)sp;
    argP[0] = argc;

    uint32_t e = ELF::load(file);
    file = nullptr;

    // Debug::printf("do we get all the way here");

    switchToUser(e, sp, 0);  // return address is handled, clear addr space.
    Debug::panic("*** implement switchToUser");
    return -10;
}

extern "C" int sysHandler(uint32_t eax, uint32_t* frame) {
    using namespace gheith;

    uint32_t* userEsp = (uint32_t*)frame[3];
    uint32_t userPC = frame[0];

    // Debug::printf("*** syscall #%d\n", eax);

    switch (eax) {
        case 0: {
            auto status = userEsp[1];
            // MISSING();
            current()->process->output->set(status);
            stop();
        }
            return 0;
        case 1: /* write */
        {
            if (userEsp[2] < 0x80000000 || userEsp[2] == 0xFEC00000 ||
                userEsp[2] == 0xFEE00000) {
                return -1;
            }
            // return # bytes written
            int fd = (int)userEsp[1];
            char* buf = (char*)userEsp[2];
            size_t nbyte = (size_t)userEsp[3];
            auto file = current()->process->getFile(fd);
            if (file == nullptr) return -1;
            return file->write(buf, nbyte);
        }
        case 2: /* fork */ {
            // return 0 for child, id for parent?

            auto parent_id = (int)(current()->id);
            // int& id_ref = parent_id;
            Shared<Process> child = current()->process->fork(parent_id);
            uint32_t esp = (uint32_t)userEsp;
            if (child != nullptr) {
                thread(child, [userPC, esp] { switchToUser(userPC, esp, 0); });
                // return 0;
            }
            return parent_id;
        }
        case 3: /* sem */
        {
            return current()->process->newSemaphore((uint32_t)userEsp[1]);
        }

        case 4: /* up */
        {
            if (current()->process->getSemaphore(userEsp[1]) == nullptr) {
                return -1;
            };
            current()->process->getSemaphore(userEsp[1])->up();
            if (current()->process->getSemaphore(userEsp[1]) == nullptr) {
                return -1;
            };
            return 0;
        }
        case 5: /* down */
        {
            if (current()->process->getSemaphore(userEsp[1]) == nullptr) {
                return -1;
            };
            current()->process->getSemaphore(userEsp[1])->down();
            if (current()->process->getSemaphore(userEsp[1]) == nullptr) {
                return -1;
            };
            return 0;
        }
        case 6: /* close */
        {
            int id = (int)userEsp[1];
            return current()->process->close(id);
        }
        case 7: /* shutdown */
            Debug::shutdown();
            return -1;

        case 8: /* wait */
        {
            if (userEsp[2] < 0x80000000) {
                return -1;
            }
            return current()->process->wait((int)userEsp[1],
                                            (uint32_t*)userEsp[2]);
        }

        case 9: /* execl */
        {
            int count = 0;
            int i = 2;
            while (userEsp[i]) {
                count++;
                i++;
            }
            if ((void*)userEsp[i - 1] == nullptr) {  // path is null
                return -1;
            }

            const char** argvMod = new const char*[count + 1];

            i = 2;
            while (userEsp[i]) {
                char* elem;
                int size = K::strlen((char*)(userEsp[i])) + 1;  // plus 1
                elem = new char[K::strlen((char*)(userEsp[i]))];
                memcpy(elem, (char*)(userEsp[i]),
                       K::strlen((char*)(userEsp[i])));
                elem[size - 1] = '\0';
                // }
                argvMod[i - 2] = elem;
                i++;
            }

            int size = K::strlen((char*)userEsp[1]) + 1;
            char* path = new char[size];

            memcpy(path, (char*)userEsp[1], size);
            int stat = SYS::exec(path, count, argvMod);

            return stat;
        }

        case 10: /* open */
        {
            char* path = (char*)userEsp[1];
            auto file = root_fs->find(root_fs->root, path);

            if (file == nullptr || path[0] == '\0') {
                return -1;
            }

            // first check for the symlink.
            if (file->is_symlink()) {
                while (file->is_symlink()) {
                    char p[file->size_in_bytes() + 1];
                    file->get_symbol(p);
                    p[file->size_in_bytes()] = '\0';
                    file = root_fs->find(root_fs->root, p);
                }
            }

            // if it's a directory, we can't do anything
            if (file == nullptr) {
                return -1;  // return -1 if directory cuz we can't do that
            }

            if (file == nullptr) {  // do it here cuz if it's null even after
                                    // all that effort, we want to return -1
                return -1;
            }

            // file is now guaranteed to be an actual file.
            ActualFile* fd = new ActualFile(file);
            return current()->process->setFile(Shared<File>(fd));
        }

        case 11: /* len */
        {
            Shared<File> file = current()->process->getFile((int)userEsp[1]);
            if (file != nullptr) {
                return file->size();
            }
            return -1;
        }

        case 12: /* read */
        {
            if (userEsp[2] < 0x80000000 || userEsp[2] == 0xFEC00000 ||
                userEsp[2] == 0xFEE00000) {
                return -1;
            } else if (userEsp[1] == 1) {
                return -1;
            }
            Shared<File> fd = current()->process->getFile(userEsp[1]);
            if (fd != nullptr) {
                int bytesRead = fd->read((uint32_t*)userEsp[2], userEsp[3]);
                return bytesRead;
            }
            return -1;
        }

        case 13: /* seek */
        {
            if (userEsp[1] == 1) {
                return -1;
            }
            Shared<File> fd = current()->process->getFile(userEsp[1]);
            return fd->seek(userEsp[2]);
        }

        case 14: /* execvp */
        {
            int i = 0;
            // while (userEsp[i]) {
            //     Debug::printf("userEsp[%d]: %s\n", i, (char*)userEsp[i]);
            //     i++;
            // }

            // Debug::printf("path: %s\n", (char*)userEsp[1]);

            int count = 0;
            i = 5;
            while (userEsp[i]) {
                count++;
                i++;
            }
            if ((void*)userEsp[i - 1] == nullptr) {  // path is null
                return -1;
            }

            const char** argvMod = new const char*[count + 1];

            i = 5;
            while (userEsp[i]) {
                char* elem;
                int size = K::strlen((char*)(userEsp[i])) + 1;  // plus 1
                elem = new char[K::strlen((char*)(userEsp[i]))];
                memcpy(elem, (char*)(userEsp[i]),
                       K::strlen((char*)(userEsp[i])));
                elem[size - 1] = '\0';
                // }
                argvMod[i - 5] = elem;
                i++;
            }

            int size = K::strlen((char*)userEsp[1]) + 1;
            char* path = new char[size];

            memcpy(path, (char*)userEsp[1], size);

            // Debug::printf("path: %s\n", path);
            // Debug::printf("count: %d\n", count);
            // int j = 0;
            // while (argvMod[j]) {
            //     Debug::printf("argvMod[%d]: %s\n", j, argvMod[j]);
            //     j++;
            // }
            argvMod[i] = nullptr;
            int stat = SYS::exec(path, count, argvMod);

            return stat;
        }

        case 15: /* opendir */
        {
            // Debug::printf("called here\n");
            char* path = (char*)userEsp[1];
            // Debug::printf("path: %s\n", path);
            auto file = root_fs->find(root_fs->root, path);

            if (!file->is_dir()) {
                Debug::printf("not a directory\n");
            } else if (file == nullptr) {
                Debug::printf("not found\n");
            }

            if (file == nullptr || path[0] == '\0') {
                return -1;
            }

            // file is now guaranteed to be an actual directory.
            ActualFile* fd = new ActualFile(file);
            int stat = current()->process->setFile(Shared<File>(fd));
            if (stat < 0) {
                Debug::printf("unable to set file in opendir\n");
            }
            return stat;
        }

        case 16: /* readdir */
        {
            Shared<File> fd = current()->process->getFile(userEsp[1]);
            fd->seek(userEsp[3]);
            int bytes = fd->read((char*) userEsp[2], 1024);
            if (bytes > 0) return (int) userEsp[2];
            else return 0;
        }

        default:
            Debug::printf("*** 1000000000 unknown system call %d\n", eax);
            return -1;
    }
}

void SYS::init(void) { IDT::trap(48, (uint32_t)sysHandler_, 3); }
