#include "concurrent_queue.hpp"

#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

void produce_values(cheetah::ConcurrentQueue<int>* queue){
    // std::vector<int> values{1, 2, 3, 4, 5};
    for(int ii = 0; ii < 50; ++ii){
        queue->push(ii);
    }
};

void consume_values(cheetah::ConcurrentQueue<int>* queue){
    std::vector<int> got;
    while(true) {
        auto maybe_value = queue->front_pop();
        if(maybe_value) got.push_back(*maybe_value);
        else break;
    }

    std::string s;
    s.append("\n");
    std::stringstream ss;
    ss<<std::this_thread::get_id();
    s += ss.str();
    for(const auto& val : got) {
        s.append(" " + std::to_string(val));
    }

    std::cout<<s<<std::flush;
    
};


TEST_CASE("concurrent_queue compiles", "[concurrent_queue]")
{
    cheetah::ConcurrentQueue<int> q;
    std::size_t no_prod = 4;
    std::vector<std::thread> producers;
    
    std::vector<std::thread> consumers;
    for(int ii = 0; ii < no_prod; ++ii){
        producers.emplace_back(std::thread(produce_values, &q));
        consumers.emplace_back(std::thread(consume_values, &q));
    }
    
    for(auto& producer : producers)
        producer.join();
    
    q.set_done();


    for(auto& consumer : consumers)
        consumer.join();

    std::cout<<std::endl;
}
