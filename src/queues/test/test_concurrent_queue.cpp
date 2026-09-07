#include "concurrent_queue.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("concurrent_queue compiles", "[concurrent_queue]")
{
    cheetah::ConcurrentQueue<int> queue;
}
