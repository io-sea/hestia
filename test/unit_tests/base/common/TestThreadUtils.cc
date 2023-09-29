#include <catch2/catch_all.hpp>
#include <thread>

#include "ThreadCollection.h"

TEST_CASE("Thread size returns correct number of threads", "[common]")
{
    ThreadCollection threads;
    REQUIRE(threads.size() == 0);
    auto new_thread = std::make_unique<std::thread>();
    threads.add(std::move(new_thread));
    REQUIRE(threads.size() == 1);
    // TODO test this more robustly
}