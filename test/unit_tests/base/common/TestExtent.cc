#include <catch2/catch_all.hpp>

#include "Extent.h"

TEST_CASE("Test Extent Operations", "[extents]")
{
    hestia::Extent ext;
    ext.m_offset = 5;
    ext.m_length = 10;

    /*

    SECTION("Extents equal")
    {
        hestia::Extent ext_equal{5, 10};
        REQUIRE(ext == ext_equal);
    }

    SECTION("Extents include")
    {
        hestia::Extent ext_inside{6, 9};
        REQUIRE(ext.includes(ext_inside));
    }

    SECTION("Extents do not include")
    {
        hestia::Extent ext_outside{4, 9};
        REQUIRE_FALSE(ext.includes(ext_outside));
        REQUIRE(ext.overlaps(ext_outside));
    }
    */
}