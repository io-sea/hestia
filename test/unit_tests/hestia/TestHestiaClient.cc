#include <catch2/catch_all.hpp>

#include "HestiaClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "CacheTestFixture.h"
#include "TestClientConfigs.h"

#include <future>
#include <iostream>

class TestHestiaClient : public hestia::HestiaClient {
  public:
    void initialize_logger() const override {}
};

class HestiaClientTestFixture : public CacheTestFixture {
  public:
    HestiaClientTestFixture()
    {
        hestia::Dictionary extra_config;
        hestia::TestClientConfigs::get_hsm_memory_client_config(extra_config);

        m_client = std::make_unique<TestHestiaClient>();
        m_client->initialize({}, {}, extra_config);
    }

    hestia::HestiaResponse::Ptr make_request(
        const hestia::HestiaRequest& req, hestia::Stream* stream = nullptr)
    {
        std::promise<hestia::HestiaResponse::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::HestiaResponse::Ptr response) {
                response_promise.set_value(std::move(response));
            };
        m_client->make_request(req, completion_cb, stream);

        if (stream != nullptr) {
            stream->flush();
        }
        return response_future.get();
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

        hestia::HestiaRequest req(hestia::HsmItem::Type::OBJECT, get_action);
        make_request(req, &stream);

        std::string returned_content(
            return_buffer.begin(), return_buffer.end());
        REQUIRE(returned_content == content);
    }

    std::unique_ptr<TestHestiaClient> m_client;
};

TEST_CASE_METHOD(HestiaClientTestFixture, "Test Hestia Client", "[hestia]")
{
    hestia::VecCrudIdentifier ids;
    hestia::CrudAttributes attributes;

    hestia::HestiaRequest create_req(
        hestia::HsmItem::Type::OBJECT, hestia::CrudMethod::CREATE);
    auto create_response = make_request(create_req);

    REQUIRE(create_response->get_ids().size() == 1);
    const auto working_id =
        create_response->get_ids().first().get_primary_key();

    REQUIRE(create_response->has_content());

    const std::string content = "The quick brown fox jumps over the lazy dog.";

    hestia::Stream stream;
    stream.set_source(hestia::InMemoryStreamSource::create(
        hestia::ReadableBufferView(content)));

    hestia::HsmAction put_action(
        hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::PUT_DATA);
    put_action.set_subject_key(working_id);
    put_action.set_target_tier(0);

    hestia::HestiaRequest put_req(hestia::HsmItem::Type::OBJECT, put_action);
    const auto put_response = make_request(put_req, &stream);

    REQUIRE_FALSE(put_response->get_action_id().empty());

    hestia::VecCrudIdentifier action_ids;
    action_ids.push_back(put_response->get_action_id());

    hestia::HestiaRequest read_req(
        hestia::HsmItem::Type::ACTION, hestia::CrudMethod::READ);
    read_req.set_id(put_response->get_action_id());
    read_req.set_output_format(hestia::CrudQuery::BodyFormat::DICT);

    const auto read_response = make_request(read_req);
    REQUIRE(read_response->has_content());

    get_and_check(working_id, 0, content);

    hestia::HsmAction copy_action(
        hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::COPY_DATA);
    copy_action.set_subject_key(working_id);
    copy_action.set_source_tier(0);
    copy_action.set_target_tier(1);

    hestia::HestiaRequest copy_req(hestia::HsmItem::Type::OBJECT, copy_action);
    make_request(copy_req);

    get_and_check(working_id, 1, content);

    hestia::HsmAction move_action(
        hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::MOVE_DATA);
    move_action.set_subject_key(working_id);
    move_action.set_source_tier(1);
    move_action.set_target_tier(2);

    hestia::HestiaRequest move_req(hestia::HsmItem::Type::OBJECT, move_action);
    make_request(move_req);

    get_and_check(working_id, 2, content);
}

TEST_CASE_METHOD(
    HestiaClientTestFixture,
    "Test Hestia Client - Attempt overwrite",
    "[hestia]")
{
    hestia::CrudIdentifierCollection ids;
    ids.add_primary_key("1234");

    hestia::HestiaRequest create_req(
        hestia::HsmItem::Type::OBJECT, hestia::CrudMethod::CREATE);
    create_req.set_ids(ids);

    auto create_response = make_request(create_req);
    REQUIRE(create_response->ok());

    auto create_response2 = make_request(create_req);
    REQUIRE_FALSE(create_response2->ok());
}
