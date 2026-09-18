#include "shrd_mem_ring_queue.hpp"

#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

void produce_values(cheetah::ShrdMemeRingQueue* queue) {
    cheetah::QProducer producer;
    producer.queue = queue;
    double value = 10;
    std::vector<std::byte> buffer(sizeof(double));
    for(int ii = 0; ii < buffer.size(); ++ii) {
        buffer[ii] = *(reinterpret_cast<std::byte*>(&value) + ii*sizeof(std::byte));
    };   
    producer.push({buffer});
};

void consume_values(cheetah::ShrdMemeRingQueue* queue) {
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // give the producer a chance
    cheetah::QConsumer consumer;
    consumer.queue = queue;

    std::vector<std::byte> buffer(sizeof(double));
    
    consumer.try_read(buffer);

    std::cout<<"Got: "<<*reinterpret_cast<double*>(buffer.data());
};

// Note this is a general test - due to unit test limitations of multi-process 
// applications we multi-thread it, which is not 100% but it will do for now.

TEST_CASE("single producer-consumer", "[shared_memory_ring_queue]")
{

    cheetah::ShrdMemeRingQueue queue;
    std::thread producer(produce_values, &queue);
    std::thread consumer(consume_values, &queue); 

    producer.join();
    consumer.join();
    
    std::cout<<std::endl;
}
