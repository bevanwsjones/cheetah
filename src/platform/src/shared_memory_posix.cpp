#include "shared_memory.h"


#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
namespace cheetah::platform::details{
    
std::optional<void*> size_and_map(const char* name, std::size_t size, const int file_descriptor) {
    if(ftruncate(file_descriptor, size) == -1) {
        std::cerr<<"Failed to size shared memory, "<<name;
        close(file_descriptor);
        return std::nullopt;
    }

    void* shrd_mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
    if(shrd_mem == MAP_FAILED) {    
        std::cerr<<"Failed to map shared memory, "<<name;
        close(file_descriptor);
        return std::nullopt;
    }

    close(file_descriptor);
    return{shrd_mem};
}

bool unmap_and_unlink(void* data, const char* name, std::size_t size) {
    if(munmap(data, size) == -1) {
        std::cerr<<"Failed to unmap, "<<name;
        return false;
    }

    data = nullptr;

    if(shm_unlink(name) == -1) {
        std::cerr<<"Failed to unlink shared memory, "<<name;
        return false;
    }
    return true;  
}


std::optional<void*> create_shared_memory(const char* name, std::size_t size){
    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if(fd == -1) {
        std::cerr<<"Failed to open shared memory, "<<name;
        return std::nullopt;
    }
    return size_and_map(name, size, fd);
}

bool destroy_shared_memory(void* data, const char* name, std::size_t size){
    return unmap_and_unlink(data, name, size); 
}

std::optional<void*> connect_shared_memory(const char* name, std::size_t size){
    int fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
    if(fd == -1) {
        std::cerr<<"Failed to open shared memory, "<<name;
        return std::nullopt;
    }
    return size_and_map(name, size, fd);
}

bool disconnect_shared_memory(void* data, const char* name, std::size_t size){
    return unmap_and_unlink(data, name, size);
}

}; //cheetah::platform::details