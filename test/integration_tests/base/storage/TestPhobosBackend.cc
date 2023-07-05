#ifdef HAS_PHOBOS

#include <catch2/catch_all.hpp>

#include "CppClientTestWrapper.h"
#include "IdGenerator.h"

class PhobosTestFixture : public CppClientTestWrapper {
  public:
    void init()
    {
        CppClientTestWrapper::init(
            "TestPhobosBackend", "hestia_phobos_tests.yaml");
    }
};

TEST_CASE_METHOD(
    PhobosTestFixture, "Test phobos backend integration", "[phobos]")
{
    init();
    hestia::DefaultIdGenerator uuid_generator;
    hestia::Uuid obj_uuid   = uuid_generator.get_uuid("phobos_test");
    hestia::hsm_uint obj_id = {obj_uuid.m_hi, obj_uuid.m_lo};

    std::string content = "The quick brown fox jumps over the lazy dog.";

    auto put_rc =
        hestia::put(obj_id, false, content.data(), 0, content.length(), 0);
    REQUIRE(put_rc == 0);

    get_and_check(obj_id, 0, content);

    // auto copy_rc = hestia::copy(obj_id, 0, 1);
    // REQUIRE(copy_rc == 0);

    // get_and_check(obj_id, 1, content);

    // auto move_rc = hestia::move(obj_id, 1, 2);
    // REQUIRE(move_rc == 0);

    // get_and_check(obj_id, 2, content);
}

#endif