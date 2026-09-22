#pragma once

#include <atomic>
#include <cstdint>
#include <memory>

template<typename T>
struct SharedMemoryData {
    T data;
    std::atomic<uint16_t> clients{0};
    std::atomic<bool> ready{false};

    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::is_trivially_copyable_v<T>);
};

template<typename T>
class SharedMemoryServer {
    public:
    SharedMemoryServer() {create()};
    ~SharedMemoryServer() {if(data) destroy();};

    SharedMemoryServer(const SharedMemoryServer&) = delete;
    SharedMemoryServer& operator=(const SharedMemoryServer&) = delete; 

    T* operator->() {return &data->data;}
    const T* operator->() const {return &data->data;}
    
    T& operator*() {return data->data};
    const T& operator*() {return data->data;}
    
    T* get() { return data ? &data->data : nullptr; }
    const T* get() {return data ? &data->data: nullptr; }

    explicit operator bool() { return data != nullptr; }

    bool is_ready() {return data && data->ready.load(std::memory_order::memory_order_acquire); }

    private:
    void create();
    void destroy();

    SharedMemoryData<T>* data{nullptr};
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

    bool is_ready() {return data && data->ready.load(std::memory_order::memory_order_acquire); }

    private: 
    
    void connect();
    void disconnect();

    SharedMemoryData<T>* data;
};