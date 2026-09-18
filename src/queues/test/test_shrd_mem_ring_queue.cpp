#include "shrd_mem_ring_queue.hpp"

#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

template<typename T> 
void produce_values(cheetah::ShrdMemeRingQueue* queue, std::span<T> buffer) {
    cheetah::QProducer producer;
    producer.queue = queue;
    
    std::vector<std::byte> byte_buffer(buffer.size()*sizeof(buffer.front()));
    for(int ii = 0; ii < byte_buffer.size(); ++ii) {
        byte_buffer[ii] = *(reinterpret_cast<std::byte*>(buffer.data()) + ii*sizeof(std::byte));
    };   

    producer.push({byte_buffer});
    producer.stop_procuding();
};

template<typename T> 
void consume_values(cheetah::ShrdMemeRingQueue* queue, std::vector<T>& buffer) {
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // give the producer a chance
    cheetah::QConsumer consumer;
    consumer.queue = queue;

    std::vector<std::byte> byte_buffer(cheetah::buff_size); 
    while(consumer.is_running()){
        auto size = consumer.try_read(byte_buffer);
        
        if(size != 0) {
            for(int i_elem = 0; i_elem < size/sizeof(T); ++i_elem) {
                buffer.push_back(*reinterpret_cast<T*>(byte_buffer.data() + i_elem*sizeof(T)));
            }
        }
    }
};

// Note this is a general test - due to unit test limitations of multi-process 
// applications we multi-thread it, which is not 100% but it will do for now.

TEST_CASE("single producer-consumer", "[shared_memory_ring_queue]")
{
    cheetah::ShrdMemeRingQueue queue;
    std::vector<double> producer_buffer_fl({10, 20, 30, 40});
    std::vector<double> consumer_buffer_fl;
    std::vector<int> producer_buffer_int({10, 20, 30, 40});
    std::vector<int> consumer_buffer_int;
    
    std::thread producer_fl([&](){produce_values<double>(&queue, producer_buffer_fl);});
    std::thread consumer_fl([&](){consume_values<double>(&queue, consumer_buffer_fl);}); 
    producer_fl.join();
    consumer_fl.join();
       
    REQUIRE(producer_buffer_fl.size() == consumer_buffer_fl.size());
    for(int ii = 0; ii < producer_buffer_fl.size(); ++ii){
        REQUIRE(producer_buffer_fl[ii] ==  consumer_buffer_fl[ii]);
    }

    std::thread producer_int([&](){produce_values<int>(&queue, producer_buffer_int);});
    std::thread consumer_int([&](){consume_values<int>(&queue, consumer_buffer_int);}); 
    producer_int.join();
    consumer_int.join();

    REQUIRE(producer_buffer_int.size() == consumer_buffer_int.size());
    for(int ii = 0; ii < producer_buffer_int.size(); ++ii){
        REQUIRE(producer_buffer_int[ii] ==  consumer_buffer_int[ii]);
    } 

}
