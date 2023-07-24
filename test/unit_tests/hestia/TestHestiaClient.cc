#include <catch2/catch_all.hpp>

#include "HestiaClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "CacheTestFixture.h"

#include <iostream>

class TestHestiaClient : public hestia::HestiaClient {
  public:
    void initialize_logger() const override {}
};

class HestiaClientTestFixture : public CacheTestFixture {
  public:
    void init(const std::string& test_name)
    {
        CacheTestFixture::do_init(__FILE__, test_name);

        m_client = std::make_unique<TestHestiaClient>();
        m_client->initialize(m_config_path);
    }

    void get_and_check(
        const std::string& id, uint8_t tier, const std::string& content)
    {
        std::vector<char> return_buffer(content.length());
        hestia::WriteableBufferView buffer_view(return_buffer);
        hestia::Stream stream;
        stream.set_sink(hestia::InMemoryStreamSink::create(buffer_view));

        hestia::HsmAction get_action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::GET_DATA);
        get_action.set_subject_key(id);
        get_action.set_source_tier(tier);

        hestia::OpStatus status;
        auto completion_cb = [&status](
                                 hestia::OpStatus ret_status,
                                 const hestia::HsmAction& action) {
            status = ret_status;
            (void)action;
        };

        m_client->do_data_io_action(get_action, &stream, completion_cb);
        (void)stream.flush();
        REQUIRE(status.ok());

        std::string returned_content(
            return_buffer.begin(), return_buffer.end());
        REQUIRE(returned_content == content);
    }

    std::unique_ptr<TestHestiaClient> m_client;
};

TEST_CASE_METHOD(HestiaClientTestFixture, "Test Hestia Client", "[hestia]")
{
    init("TestHestiaClient");

    hestia::VecCrudIdentifier ids;
    hestia::CrudAttributes attributes;
    auto status =
        m_client->create(hestia::HsmItem::Type::OBJECT, ids, attributes);
    REQUIRE(status.ok());
    REQUIRE(ids.size() == 1);
    REQUIRE(attributes.has_content());
    REQUIRE(attributes.is_json());

    std::cout << ids[0].get_primary_key() << std::endl;
    std::cout << attributes.get_buffer() << std::endl;

    const std::string content = "The quick brown fox jumps over the lazy dog.";

    hestia::Stream stream;
    stream.set_source(hestia::InMemoryStreamSource::create(
        hestia::ReadableBufferView(content)));

    hestia::HsmAction put_action(
        hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::PUT_DATA);
    put_action.set_subject_key(ids[0].get_primary_key());
    put_action.set_target_tier(0);
    auto completion_cb = [&status](
                             hestia::OpStatus ret_status,
                             const hestia::HsmAction& action) {
        status = ret_status;
        (void)action;
    };
    m_client->do_data_io_action(put_action, &stream, completion_cb);
    (void)stream.flush();

    REQUIRE(status.ok());

    get_and_check(ids[0].get_primary_key(), 0, content);

    hestia::HsmAction copy_action(
        hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::COPY_DATA);
    copy_action.set_subject_key(ids[0].get_primary_key());
    copy_action.set_source_tier(0);
    copy_action.set_target_tier(1);

    status = m_client->do_data_movement_action(copy_action);
    REQUIRE(status.ok());

    get_and_check(ids[0].get_primary_key(), 1, content);

    hestia::HsmAction move_action(
        hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::MOVE_DATA);
    move_action.set_subject_key(ids[0].get_primary_key());
    move_action.set_source_tier(1);
    move_action.set_target_tier(2);

    status = m_client->do_data_movement_action(move_action);
    REQUIRE(status.ok());
    get_and_check(ids[0].get_primary_key(), 2, content);
}