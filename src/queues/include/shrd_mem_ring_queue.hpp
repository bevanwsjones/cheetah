

#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <new>
#include <span>

const std::size_t buff_size = 8*100; // size in bytes

#ifdef __cpp_lib_hardware_interference_size
constexpr std::size_t cache_line = std::hardware_destructive_interference_size;
#else
constexpr std::size_t cache_line = 64; // sane fallback
#endif

class ShrdMemeRingQueue{

    public:

    ShrdMemeRingQueue() = default;
    ~ShrdMemeRingQueue() = default;

    void push(std::span<std::byte> buff){
        const std::size_t size = buff.size();

        write_pos.store(current_pos + sizeof(size) + size, std::memory_order::release); // wrap around?
        
        std::memcpy(&buff[current_pos], &size, sizeof(size));
        std::memcpy(&buff[current_pos + sizeof(size)], buff.data(), buff.size());

        read_pos.store(current_pos + sizeof(size) + size, std::memory_order::release);
    }

    void front_pop(std::span<std::byte> buff){

    }



    private:
    
    std::atomic<std::size_t> write_pos;
    char pad [cache_line - sizeof(std::atomic<std::size_t>)];
    std::atomic<std::size_t> read_pos;

    std::size_t current_pos;
    std::array<std::byte, 100> buffer;
    
    

};