#pragma once

#include <atomic>
#include <cstdint>
#include <chrono>
#include <memory>
#include <thread>
#include <optional>
#include <string>
#include <utility>
#include <iostream>

std::optional<void*> create_shared_memory(const char* name, std::size_t size);
bool destroy_shared_memory(void* data, const char* name, std::size_t size);
std::optional<void*> connect_shared_memory(const char* name, std::size_t size);
bool disconnect_shared_memory(const char* name, std::size_t size);

template<typename T>
struct SharedMemoryData {
    T data;
    std::atomic<uint16_t> clients{0};
    std::atomic<bool> valid{false};

    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<bool>::is_always_lock_free);
    static_assert(std::is_trivially_copyable_v<T>);
};

template<typename T>
class SharedMemoryServer {
    public:
    SharedMemoryServer() = default;
    explicit SharedMemoryServer(std::string_view name) { create(name) ; }
    ~SharedMemoryServer() {if(data) destroy();};

    SharedMemoryServer(const SharedMemoryServer&) = delete;
    SharedMemoryServer& operator=(const SharedMemoryServer&) = delete; 

    SharedMemoryServer(SharedMemoryServer&& other) noexcept 
    : data(std::exchange(other.data, nullptr)), shrd_mem_name(std::move(other.shrd_mem_name)) {};
    SharedMemoryServer& operator=(SharedMemoryServer&&) noexcept {
        if(this != &other) {
            destroy();
            this->data = std::exchange(other.data, nullptr);
            this->shrd_mem_name = std::move(other.shrd_mem_name);
        }
        return *this;
    }; 

    T* operator->() {return &data->data;}
    const T* operator->() const {return &data->data;}
    
    T& operator*() {return data->data;}
    const T& operator*() const {return data->data;}
    
    T* get() { return data ? &data->data : nullptr; }
    const T* get() const {return data ? &data->data: nullptr; }

    explicit operator bool() const { return data != nullptr; }

    bool is_ready() const {return data && data->valid.load(std::memory_order_acquire); }

    private:
    bool create(std::string_view name) {
        shrd_mem_name = name;
        auto shrd_mem = create_shared_memory(shrd_mem_name.c_str(), sizeof(SharedMemoryData<T>));
        if(!shrd_mem) return false;
        data = new (shrd_mem.value()) SharedMemoryData<T>();
        data->valid.store(true, std::memory_order_release);
        return true;
    }

    void destroy(){
        data->valid.store(true, std::memory_order_release);
        uint32_t count = data->clients.load(std::memory_order_acquire);
        if(count) {
            std::cout<<"\nWaiting for "<<count<<" clients to release shared memory: "<<shrd_mem_name<<", waiting seconds.";
            const auto start_time = std::chrono::steady_clock::now();
            while(data->clients.load(std::memory_order_acquire) && 
                  wait_time > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time)){
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            if(data->clients.load(std::memory_order_acquire)){
                std::cout<<"\nWait failed, closing shared memory, "<<shrd_mem_name<<".";
            }
        }

        data->~SharedMemoryData<T>();       // explicit dtor, since placement-new was used
        destroy_shared_memory(data, shrd_mem_name, sizeof(SharedMemoryData<T>));
    };

    SharedMemoryData<T>* data{nullptr};
    std::string shrd_mem_name;
    static constexpr std::chrono::seconds wait_time{60}; 
};

template<typename T>
class SharedMemoryClient {

    public:
    SharedMemoryClient() {connect()};
    ~SharedMemoryClient() {if(data) disconnect();};

    SharedMemoryClient(const SharedMemoryClient&) = delete;
    SharedMemoryClient& operator=(const SharedMemoryClient&) = delete; 

    T* operator->() {return &data->data;}
    const T* operator->() const {return &data->data;}
    
    T& operator*() {return data->data};
    const T& operator*() {return data->data;}
    
    T* get() { return data ? &data->data : nullptr; }
    const T* get() {return data ? &data->data: nullptr; }

    explicit operator bool() { return data != nullptr; }

    bool is_ready() {return data && data->valid.load(std::memory_order::memory_order_acquire); }

    private: 
    
    void connect();
    void disconnect();

    SharedMemoryData<T>* data;
};
