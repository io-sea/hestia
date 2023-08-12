#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "CrudQuery.h"
#include "CrudRequest.h"
#include "EventSink.h"

#include "MockCrudService.h"
#include "MockModel.h"
#include "TypedCrudRequest.h"

#include "Dictionary.h"

#include "Logger.h"

class TestCrudEventSinkFixture {
  public:
    TestCrudEventSinkFixture()
    {
        m_service = hestia::mock::MockCrudService::create();
    }

    hestia::mock::MockCrudService::Ptr m_service;
};

TEST_CASE_METHOD(
    TestCrudEventSinkFixture, "Crud event sink test", "[event-feed]")
{
    hestia::CrudEventBufferSink sink;

    hestia::mock::MockModel named_model("model_id");
    named_model.set_name("model_name");
    named_model.m_my_field.update_value("field_value");

    auto request_1 = hestia::TypedCrudRequest<hestia::mock::MockModel>{
        hestia::CrudMethod::CREATE,
        named_model,
        {},
        hestia::CrudQuery::OutputFormat::ITEM};
    auto response_1 = m_service->make_request(request_1);
    REQUIRE(response_1->ok());
    sink.on_event({request_1.method(), request_1, *response_1});

    m_service->m_mock_time_provider->increment();

    hestia::mock::MockModel update_model("model_id");
    update_model.m_my_field.update_value("field_value_updated");

    auto request_2 = hestia::TypedCrudRequest<hestia::mock::MockModel>{
        hestia::CrudMethod::UPDATE,
        update_model,
        {},
        hestia::CrudQuery::OutputFormat::ITEM};
    auto response_2 = m_service->make_request(request_2);
    REQUIRE(response_2->ok());

    sink.on_event({request_2.method(), request_2, *response_2});

    LOG_INFO(sink.get_buffer());
}

// TEST_CASE("Crud file event sink test", "[event-feed]")
// {
//     REQUIRE(false);
// }