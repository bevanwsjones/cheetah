#include "concurrent_queue.hpp"

#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>

#include <iostream>

void produce_values(cheetah::ConcurrentQueue<int>* queue){
    std::vector<int> values{1, 2, 3, 4, 5};
    for(const auto& val : values){
        queue->push(val);
    }
};

void consume_values(cheetah::ConcurrentQueue<int>* queue){
    while(true){
        auto maybe_value = queue->front_pop();
        if(maybe_value) std::cout<<"\n"<<*maybe_value;
        else break;
    }
};


TEST_CASE("concurrent_queue compiles", "[concurrent_queue]")
{
    cheetah::ConcurrentQueue<int> q;
    std::thread producer(produce_values, &q);
    std::thread consumer(consume_values, &q);

    producer.join();
    q.set_done();
    consumer.join();

}
