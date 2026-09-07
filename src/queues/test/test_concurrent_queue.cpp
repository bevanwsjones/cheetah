#include "concurrent_queue.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("concurrent_queue compiles", "[concurrent_queue]")
{
    cheetah::concurrent_queue<int> queue;
}
