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

int SYS::exec(const char* path, int argc, const char* argv[]) {
    using namespace gheith;
    ElfHeader elfheader;

    auto file = root_fs->find(root_fs->root, path);

    if (file == nullptr) {
        return -1;
    }
    if (!file->is_file()) {
        return -1;
    }

    file->read(0, elfheader);

    // elf checks
    if (elfheader.maigc0 != 0x7F || elfheader.magic1 != 'E' ||
        elfheader.magic2 != 'L' || elfheader.magic3 != 'F') {
        // Debug::printf("Catching it\n");
        return -1;
    } else if (elfheader.encoding != 1) {
        return -1;
    } else if (elfheader.cls != 1) {
        return -1;
    } else if (elfheader.abi != 0) {
        return -1;
    } else if (elfheader.type != 2) {
        return -1;
    } else if (elfheader.version != 1) {
        return -1;
    } else if (elfheader.phoff == 0) {
        return -1;
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
        Debug::printf("string = %s\n", elem);
        sp -= size;
        Debug::printf("new location of sp: %x\n", sp);
        addresses[num - 1] = sp;
        // Debug::printf("addresses[%d] = %x\n", num - 1, sp);

        char* pointer = (char*) sp;
        memcpy(pointer, elem, size);
        Debug::printf("string = %s\n", pointer);
        num--;
    }

    sp -= 4;
    Debug::printf("null pointer starts at %x\n", sp);
    bzero((char*) sp, 4);

    for (int i = argc - 1; i >= 0; i--) {
        sp -= 4;
        Debug::printf("addresses[%d] = %x\n", i, addresses[i]);
        uint32_t* argvP = (uint32_t*) sp;
        Debug::printf("new sp: %x\n", sp);
        argvP[0] = addresses[i];
        Debug::printf("make sure addr went on stack: %x\n", argvP[0]);
    }

    sp -= 4;
    uint32_t* argP = (uint32_t*) sp;
    argP[0] = (sp + 4);
    Debug::printf("where does argv start: %x\n", argP[0]);
    Debug::printf("new sp: %x\n", sp);

    sp -= 4;
    argP = (uint32_t*) sp;
    argP[0] = argc;
    Debug::printf("what is argc: %d\n", argP[0]);
    Debug::printf("new sp: %x\n", sp);



    // Copy arguments

    // add argv elements to stack
    // int numElements = argc;
    // uint32_t addresses[argc];
    // while (numElements > 0) {
    //     int sizeToSubtract = K::strlen(argv[numElements - 1]) + 1;
    //     // Debug::printf("size to subtract: %d\n", sizeToSubtract);
    //     sp -=
    //         (sizeToSubtract +
    //          (4 - (sizeToSubtract % 4)));  // sp points at beginning of wherever
    //                                        // the element should start
    //     Debug::printf("elem: %x\n", sp);
    //     Debug::printf("elemenets: %s\n", argv[numElements - 1]);
    //     addresses[numElements - 1] = sp;  // assume this is right
    //     const char* elem = argv[numElements - 1];
    //     char* stackPointer = (char*)sp;
    //     memcpy(stackPointer, elem,
    //            sizeToSubtract);  // hopefully this actually adds value to stack
    //     numElements--;
    // }

    // sp -= 4;
    // bzero((char*) sp, 4);
    // Debug::printf("null: %x\n", sp);
    // char buffer[4];

    // 4 byte aligned, add a 4 byte null
    // sp -= 4;
    // char buffer[4];
    // char* stackPointer = (char*)sp;
    // memcpy(stackPointer, buffer, 4);

    // add pointers of strings to the stack
    // pointers point to where values are on the stack
    // assuming the addresses are right
    // numElements = argc - 1;
    // for (int i = numElements; i >= 0; i--) {
    //     sp -= 4;
    //     uint32_t* stackP = (uint32_t*) sp;
    //     stackP[0] = addresses[0];
    // }



    // sp -= (argc * 4);
    // stackPointer = (char*)sp;
    // Debug::printf("where does addresses start? %x\n", sp + 4);
    // Debug::printf("first elem in addresses %x\n", addresses[0]);
    // memcpy(stackPointer, (char*) (sp + 4), argc * 4);
    // Debug::printf("argv pointers: %x\n", sp);

    // // copy the address for argv
    // sp -= 4;
    // // uint32_t addr = (uint32_t)&argv;
    // char* stackPointer = (char*)sp;
    // // Debug::printf("what does argv hold? %x\n", addr);
    // memcpy(stackPointer, (char*) (sp + 4), 4);
    // Debug::printf("argv pointer: %x\n", sp);

    // // copy the argc to the stack
    // sp -= 4;
    // uint32_t* stackP = (uint32_t*)sp;
    // stackP[0] = argc;
    // Debug::printf("argc: %x\n", sp);

    uint32_t e = ELF::load(file);
    file = nullptr;

    switchToUser(e, sp, 0);  // return address is handled, clear addr space.
    Debug::panic("*** implement switchToUser");
    return -1;
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
            if (userEsp[2] < 0x80000000 || userEsp[2] == 0xFEC00000 || userEsp[2] == 0xFEE00000) {
                return -1;
            }
            // return # bytes written
            int fd = (int)userEsp[1];
            // Debug::printf("fd: %d\n", fd);
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
            if ((void*) userEsp[i - 1] == nullptr) {
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
                // Debug::printf("elem = %s\n", elem);
                argvMod[i - 2] = elem;
                i++;
            }

            int size = K::strlen((char*)userEsp[1]) + 1;
            char* path = new char[size];
            // Debug::printf("what is the length: %d\n", size);

            memcpy(path, (char*)userEsp[1], size);

            // current()->process->clear_private();
            // Debug::printf("path: %s\n", path);
            // Debug::printf("what is the length: %d\n", size);
            // path[size - 1] = '\0';
            // Debug::printf("path: %s\n", path);
            SYS::exec(path, count, argvMod);

            return -1;
        }

        case 10: /* open */
        {
            char* path = (char*)userEsp[1];
            auto file = root_fs->find(root_fs->root, path);
            
            // Debug::printf("path: %s\n", path);
            // Debug::printf("is file existant? %s\n", file == nullptr ? "no" : "yes");

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
                // Debug::printf("do we get here\n");
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
            // Debug::printf("what is this: %x\n", userEsp[2]);
            if (userEsp[2] < 0x80000000 || userEsp[2] == 0xFEC00000 || userEsp[2] == 0xFEE00000) {
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

        default:
            Debug::printf("*** 1000000000 unknown system call %d\n", eax);
            return -1;
    }
}

void SYS::init(void) { IDT::trap(48, (uint32_t)sysHandler_, 3); }
