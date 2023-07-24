#include <catch2/catch_all.hpp>

#include "LockableModel.h"

#include <iostream>

TEST_CASE("Test Lockable Model", "[models]")
{
    hestia::LockableModel lockable("my_lockable");
}