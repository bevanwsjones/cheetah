

#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <new>
#include <span>
#include <numeric>


namespace cheetah
{

constexpr std::size_t buff_size = 100; // size in bytes
constexpr std::uint32_t wrap_sentinel = 0; 
constexpr std::uint32_t stop_sentinel = std::numeric_limits<std::uint32_t>::max(); 

#ifdef __cpp_lib_hardware_interference_size
constexpr std::size_t cache_line = std::hardware_destructive_interference_size;
#else
constexpr std::size_t cache_line = 64; // sane fallback
#endif

struct ShrdMemeRingQueue{
    ShrdMemeRingQueue() = default;
    ~ShrdMemeRingQueue() = default;

    std::atomic<std::size_t> write_pos;
    char pad1 [cache_line - sizeof(std::atomic<std::size_t>)];
    std::atomic<std::size_t> read_pos;
    char pad2 [cache_line - sizeof(std::atomic<std::size_t>)];
    std::array<std::byte, buff_size> buffer;
};

struct QProducer{

    ShrdMemeRingQueue* queue;
    std::size_t current_pos{0};
    std::size_t next_element{0};

    void push(std::span<std::byte> buff){
        const uint32_t payload_size = sizeof(uint32_t) + buff.size();
    
        if((next_element + payload_size) < (queue->buffer.size() - sizeof(uint32_t))) [[likely]] {
            current_pos += payload_size;
        }
        else {
            std::memcpy(&queue->buffer[next_element], &wrap_sentinel, sizeof(uint32_t));
            next_element = 0;
            current_pos = payload_size;
        }

        queue->write_pos.store(current_pos, std::memory_order::release); 
        std::memcpy(&queue->buffer[next_element], &payload_size, sizeof(uint32_t));
        std::memcpy(&queue->buffer[next_element + sizeof(uint32_t)], buff.data(), buff.size());

        queue->read_pos.store(current_pos, std::memory_order::release);
        next_element += payload_size;       
    }

    void stop_procuding(){
        const uint32_t payload_size = sizeof(stop_sentinel);
        if((next_element + payload_size) < (queue->buffer.size() - sizeof(uint32_t))) [[likely]] {
            current_pos += payload_size;
        }
        else {
            std::memcpy(&queue->buffer[next_element], &wrap_sentinel, sizeof(uint32_t));
            next_element = 0;
            current_pos = payload_size;
        }

        queue->write_pos.store(current_pos, std::memory_order::release); 
        std::memcpy(&queue->buffer[next_element], &stop_sentinel, sizeof(uint32_t));
        queue->read_pos.store(current_pos, std::memory_order::release);
        next_element += payload_size;  
    }
};

struct QConsumer{

    ShrdMemeRingQueue* queue;
    bool queue_running{true};
    std::size_t current_pos{0};
    std::size_t next_element{0};

    // wrap around?
    uint32_t try_read(std::span<std::byte> buff){

        if(!queue_running || next_element == queue->read_pos.load(std::memory_order::acquire))
            return 0;

        uint32_t payload_size;
        std::memcpy(&payload_size, &queue->buffer[next_element], sizeof(uint32_t)); 
        
        if(wrap_sentinel == payload_size) [[unlikely]] { // buffer wrapped, reset ring
            next_element = 0;
            std::memcpy(&payload_size, &queue->buffer[next_element], sizeof(uint32_t)); 
        }

        if(stop_sentinel == payload_size) [[unlikely]] { // we must stop
            queue_running = false;
            return 0;
        }

        // error check with writer pos?
        const uint32_t size = payload_size - sizeof(uint32_t);
        std::memcpy(buff.data(), &queue->buffer[next_element + sizeof(uint32_t)], size); 

        next_element += payload_size;
        return size;
    }

    inline bool is_running() const {
        return queue_running;
    }
};

}// cheetah
