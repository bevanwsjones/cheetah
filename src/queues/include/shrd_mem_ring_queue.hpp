

#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <new>
#include <span>

constexpr std::size_t buff_size = 8*100; // size in bytes

#ifdef __cpp_lib_hardware_interference_size
constexpr std::size_t cache_line = std::hardware_destructive_interference_size;
#else
constexpr std::size_t cache_line = 64; // sane fallback
#endif

struct ShrdMemeRingQueue{
    ShrdMemeRingQueue() = default;
    ~ShrdMemeRingQueue() = default;

    std::atomic<std::size_t> write_pos;
    char pad [cache_line - sizeof(std::atomic<std::size_t>)];
    std::atomic<std::size_t> read_pos;
    char pad [cache_line - sizeof(std::atomic<std::size_t>)];
    std::array<std::byte, buff_size> buffer;
};

struct QProducer{

    ShrdMemeRingQueue* queue;
    std::size_t current_pos{0};
    std::size_t next_element{0};

    // wrap around?
    void push(std::span<std::byte> buff){
        const uint32_t payload_size = sizeof(uint32_t) + buff.size();
                
        if((next_element + payload_size) < queue->buffer.size()) [[likely]] {
            current_pos += payload_size;
        }
        else {
            next_element = 0;
            current_pos = payload_size;
        }

        queue->write_pos.store(current_pos, std::memory_order::release); 
        std::memcpy(&queue->buffer[next_element], &payload_size, sizeof(uint32_t));
        std::memcpy(&queue->buffer[next_element + sizeof(uint32_t)], buff.data(), buff.size());

        queue->read_pos.store(current_pos, std::memory_order::release);
        next_element += payload_size;
    }
};

struct QConsumer{

    ShrdMemeRingQueue* queue;
    std::size_t current_pos{0};
    std::size_t next_element{0};

    // wrap around?
    uint32_t try_read(std::span<std::byte> buff){
        const std::size_t read_pos = queue->read_pos.load(std::memory_order::acquire);
        if(next_element == read_pos)
            return 0;

        if(next_element > read_pos) [[unlikely]] // buffer wrapped.
            next_element = 0;

        uint32_t size;
        std::memcpy(&size, &queue->buffer[next_element], sizeof(uint32_t)); 

        // error check with writer pos?

        std::memcpy(buff.data(), &queue->buffer[next_element + sizeof(uint32_t)], size); 

        int payload_size = sizeof(uint32_t) + size; 
        next_element += payload_size;
    }

};