#include <catch2/catch_all.hpp>

#include "JsonUtils.h"
#include "MockCrudService.h"
#include "MockModel.h"
#include "TypedCrudRequest.h"

#include <iostream>

class TestCrudServiceFixture {
  public:
    TestCrudServiceFixture()
    {
        m_service = hestia::mock::MockCrudService::create();
        m_mock_with_parent_service =
            hestia::mock::MockCrudService::create_mock_with_parent(
                m_service->m_kv_store_client.get());
        m_parent_service = hestia::mock::MockCrudService::create_for_parent(
            m_service->m_kv_store_client.get());
        m_many_many_parent_service =
            hestia::mock::MockCrudService::create_many_many_parent(
                m_service->m_kv_store_client.get());
    }
    hestia::mock::MockCrudService::Ptr m_service;
    hestia::mock::MockCrudService::Ptr m_mock_with_parent_service;
    hestia::mock::MockCrudService::Ptr m_parent_service;
    hestia::mock::MockCrudService::Ptr m_many_many_parent_service;
};

TEST_CASE_METHOD(TestCrudServiceFixture, "Test Crud Service", "[crud-service]")
{
    const auto create_response = m_service->make_request(hestia::CrudRequest{
        hestia::CrudMethod::CREATE,
        {},
        {},
        {},
        hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(create_response->ok());

    REQUIRE(create_response->items().size() == 1);

    auto created_model =
        create_response->get_item_as<hestia::mock::MockModel>();
    REQUIRE(created_model->m_my_field.get_value() == "default_field");

    m_service->m_mock_time_provider->increment();

    hestia::mock::MockModel named_model;
    named_model.set_name("model_name");
    named_model.m_my_field.update_value("field_value");

    const auto create_response1 = m_service->make_request(
        hestia::TypedCrudRequest<hestia::mock::MockModel>{
            hestia::CrudMethod::CREATE,
            named_model,
            {},
            hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(create_response1->ok());

    // std::cout << m_kv_store_client.dump() << std::endl;

    auto created_model1 =
        create_response1->get_item_as<hestia::mock::MockModel>();
    REQUIRE(created_model1->get_creation_time() == 1);
    REQUIRE(created_model1->name() == "model_name");
    REQUIRE(created_model1->m_my_field.get_value() == "field_value");

    WHEN("Getting using id")
    {
        WHEN("the item exists")
        {
            hestia::CrudQuery query(
                hestia::CrudIdentifier("1"),
                hestia::CrudQuery::OutputFormat::ITEM);
            const auto get_response =
                m_service->make_request(hestia::CrudRequest{query, {}});
            THEN("It is found ok")
            {
                REQUIRE(get_response->ok());
                REQUIRE(get_response->found());
            }

            hestia::CrudQuery query1(
                create_response1->get_item()->id(),
                hestia::CrudQuery::OutputFormat::ITEM);

            const auto get_response1 =
                m_service->make_request(hestia::CrudRequest{query1, {}});
            REQUIRE(get_response1->ok());
            REQUIRE(get_response1->found());

            REQUIRE(get_response1->get_item()->get_creation_time() == 1);
            REQUIRE(get_response1->get_item()->name() == "model_name");
        }

        WHEN("the item doesn't exist")
        {
            hestia::CrudQuery query(
                hestia::CrudIdentifier("3"),
                hestia::CrudQuery::OutputFormat::ITEM);
            const auto get_response =
                m_service->make_request(hestia::CrudRequest{query, {}});
            THEN("It is not found ok")
            {
                REQUIRE(get_response->ok());
                REQUIRE_FALSE(get_response->found());
            }
        }
    }

    WHEN("Getting using multiple ids")
    {
        hestia::VecCrudIdentifier ids;
        ids.push_back(create_response->get_item()->id());
        ids.push_back(create_response1->get_item()->id());

        hestia::CrudQuery query(ids, hestia::CrudQuery::OutputFormat::ITEM);

        const auto get_response =
            m_service->make_request(hestia::CrudRequest{query, {}});
        THEN("The correct number are found")
        {
            REQUIRE(get_response->ok());
            REQUIRE(get_response->items().size() == 2);
        }
    }

    WHEN("Getting using an index field")
    {
        WHEN("the field is valid")
        {
            hestia::CrudQuery query(
                hestia::KeyValuePair{"name", "model_name"},
                hestia::CrudQuery::Format::GET,
                hestia::CrudQuery::OutputFormat::ITEM);
            const auto get_response =
                m_service->make_request(hestia::CrudRequest{query, {}});
            THEN("It is found ok")
            {
                REQUIRE(get_response->ok());
                REQUIRE(get_response->found());
            }
        }

        WHEN("the field is not valid")
        {
            hestia::CrudQuery query(
                hestia::KeyValuePair{"name", "no_model_has_this_name"},
                hestia::CrudQuery::Format::GET,
                hestia::CrudQuery::OutputFormat::ITEM);
            const auto get_response =
                m_service->make_request(hestia::CrudRequest{query, {}});
            THEN("It is not found ok")
            {
                REQUIRE(get_response->ok());
                REQUIRE_FALSE(get_response->found());
            }
        }
    }

    WHEN("Getting without a query")
    {
        hestia::CrudQuery query(hestia::CrudQuery::OutputFormat::ITEM);
        const auto get_response =
            m_service->make_request(hestia::CrudRequest{query, {}});
        THEN("All items are found")
        {
            REQUIRE(get_response->ok());
            REQUIRE(get_response->items().size() == 2);
        }
    }
}

TEST_CASE_METHOD(
    TestCrudServiceFixture, "Test Crud Service - Update", "[crud-service]")
{
    const auto create_response = m_service->make_request(hestia::CrudRequest{
        hestia::CrudMethod::CREATE,
        {},
        {},
        hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(create_response->ok());

    REQUIRE(create_response->items().size() == 1);

    auto created_model =
        create_response->get_item_as<hestia::mock::MockModel>();
    REQUIRE(created_model->m_my_field.get_value() == "default_field");

    m_service->m_mock_time_provider->increment();

    hestia::mock::MockModel named_model;
    named_model.set_name("model_name");
    named_model.m_my_field.update_value("field_value");

    const auto create_response1 = m_service->make_request(
        hestia::TypedCrudRequest<hestia::mock::MockModel>{
            hestia::CrudMethod::CREATE,
            named_model,
            {},
            hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(create_response1->ok());

    // std::cout << m_kv_store_client.dump() << std::endl;

    auto created_model1 =
        create_response1->get_item_as<hestia::mock::MockModel>();
    REQUIRE(created_model1->get_creation_time() == 1);
    REQUIRE(created_model1->name() == "model_name");
    REQUIRE(created_model1->m_my_field.get_value() == "field_value");

    WHEN("Updating an item")
    {
        m_service->m_mock_time_provider->increment();

        hestia::mock::MockModel model_to_update(
            create_response1->get_item()->id());
        model_to_update.m_my_field.update_value("updated_field_value");

        const auto update_response = m_service->make_request(
            hestia::TypedCrudRequest<hestia::mock::MockModel>{
                hestia::CrudMethod::UPDATE,
                model_to_update,
                {},
                hestia::CrudQuery::OutputFormat::ITEM});
        REQUIRE(update_response->ok());

        THEN("It reflects the update")
        {
            REQUIRE(update_response->found());

            auto my_updated_model =
                update_response->get_item_as<hestia::mock::MockModel>();

            REQUIRE(my_updated_model->get_last_modified_time() == 2);
            REQUIRE(my_updated_model->name() == "model_name");
            REQUIRE(
                my_updated_model->m_my_field.get_value()
                == "updated_field_value");
        }
    }
}

TEST_CASE_METHOD(
    TestCrudServiceFixture, "Test Crud Service - Read", "[crud-service]")
{
    const auto create_response = m_service->make_request(hestia::CrudRequest{
        hestia::CrudMethod::CREATE,
        {},
        {},
        {},
        hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(create_response->ok());

    REQUIRE(create_response->items().size() == 1);

    auto created_model =
        create_response->get_item_as<hestia::mock::MockModel>();
    REQUIRE(created_model->m_my_field.get_value() == "default_field");

    m_service->m_mock_time_provider->increment();

    hestia::mock::MockModel named_model;
    named_model.set_name("model_name");
    named_model.m_my_field.update_value("field_value");

    const auto create_response1 = m_service->make_request(
        hestia::TypedCrudRequest<hestia::mock::MockModel>{
            hestia::CrudMethod::CREATE,
            named_model,
            {},
            hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(create_response1->ok());

    // std::cout << m_kv_store_client.dump() << std::endl;

    auto created_model1 =
        create_response1->get_item_as<hestia::mock::MockModel>();
    REQUIRE(created_model1->get_creation_time() == 1);
    REQUIRE(created_model1->name() == "model_name");
    REQUIRE(created_model1->m_my_field.get_value() == "field_value");

    WHEN("Listing items")
    {
        WHEN("there is no filter")
        {
            hestia::CrudQuery query(hestia::CrudQuery::OutputFormat::ID);
            const auto list_response =
                m_service->make_request(hestia::CrudRequest{query, {}});
            THEN("all items are returned")
            {
                REQUIRE(list_response->ok());
                REQUIRE(list_response->ids().size() == 2);
            }
        }

        WHEN("there is a filter")
        {
            hestia::CrudQuery query(
                hestia::KeyValuePair{"name", "model_name"},
                hestia::CrudQuery::Format::GET,
                hestia::CrudQuery::OutputFormat::ID);

            const auto list_response =
                m_service->make_request(hestia::CrudRequest{query, {}});
            THEN("a matching item is returned")
            {
                REQUIRE(list_response->ok());
                REQUIRE(list_response->ids().size() == 1);
            }
        }
    }
}

TEST_CASE_METHOD(
    TestCrudServiceFixture, "Test Crud Service - Create", "[crud-service]")
{
    WHEN("Item is Created with no other inputs")
    {
        const auto response = m_service->make_request(hestia::CrudRequest{
            hestia::CrudMethod::CREATE,
            {},
            {},
            {},
            hestia::CrudQuery::OutputFormat::ITEM});

        THEN("It is created ok and returns an item")
        {
            REQUIRE(response->ok());
            REQUIRE(response->items().size() == 1);

            auto model = response->get_item_as<hestia::mock::MockModel>();
            REQUIRE(model->m_my_field.get_value() == "default_field");
        }
    }

    m_service->m_mock_time_provider->increment();

    WHEN("Item is Created using an existing Item instance")
    {
        hestia::mock::MockModel named_model;
        named_model.set_name("model_name");
        named_model.m_my_field.update_value("field_value");

        const auto response = m_service->make_request(
            hestia::TypedCrudRequest<hestia::mock::MockModel>{
                hestia::CrudMethod::CREATE,
                named_model,
                {},
                hestia::CrudQuery::OutputFormat::ITEM});

        THEN("It is created ok and returns an item")
        {
            REQUIRE(response->ok());

            auto model = response->get_item_as<hestia::mock::MockModel>();
            REQUIRE(model->get_creation_time() == 1);
            REQUIRE(model->name() == "model_name");
            REQUIRE(model->m_my_field.get_value() == "field_value");
        }
    }

    WHEN("Item is Created using Attributes")
    {
        hestia::mock::MockModel named_model;
        named_model.set_name("model_name");
        named_model.m_my_field.update_value("field_value");

        hestia::CrudAttributes attributes;

        auto model_factory = hestia::mock::MockModel::create_factory();
        hestia::JsonAdapter adapter(model_factory.get());

        hestia::Dictionary dict;
        named_model.serialize(dict);

        std::string content;
        adapter.dict_to_string(dict, content);
        attributes.set_buffer(content);

        const auto response = m_service->make_request(hestia::CrudRequest{
            hestia::CrudMethod::CREATE,
            {},
            {},
            attributes,
            hestia::CrudQuery::OutputFormat::ITEM});


        THEN("It is created ok and returns an item")
        {
            REQUIRE(response->ok());

            auto model = response->get_item_as<hestia::mock::MockModel>();
            REQUIRE(model->name() == "model_name");
            REQUIRE(model->m_my_field.get_value() == "field_value");
        }
    }

    WHEN("Items are created with multiple ids")
    {
        hestia::VecCrudIdentifier ids;
        ids.push_back(hestia::CrudIdentifier("4"));
        ids.push_back(hestia::CrudIdentifier("5"));
        ids.push_back(hestia::CrudIdentifier("6"));

        const auto response = m_service->make_request(hestia::CrudRequest{
            hestia::CrudMethod::CREATE,
            {},
            ids,
            {},
            hestia::CrudQuery::OutputFormat::ID});

        THEN("The correct number of items are created")
        {
            REQUIRE(response->ok());
            REQUIRE(response->ids().size() == 3);
        }
    }
}

TEST_CASE_METHOD(
    TestCrudServiceFixture,
    "Test Crud Service - Foreign Keys",
    "[crud-service]")
{
    hestia::mock::MockParentModel parent_model;
    const auto response = m_parent_service->make_request(
        hestia::TypedCrudRequest<hestia::mock::MockParentModel>{
            hestia::CrudMethod::CREATE,
            parent_model,
            {},
            hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(response->ok());

    hestia::mock::MockManyToManyTargetModel many_many_target_model;
    const auto many_target_response = m_many_many_parent_service->make_request(
        hestia::TypedCrudRequest<hestia::mock::MockManyToManyTargetModel>{
            hestia::CrudMethod::CREATE,
            many_many_target_model,
            {},
            hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(many_target_response->ok());

    hestia::mock::MockModelWithParent model;
    model.set_parent_id(response->get_item()->id());
    model.add_many_many_id(many_target_response->get_item()->id());

    auto child_response = m_mock_with_parent_service->make_request(
        hestia::TypedCrudRequest<hestia::mock::MockModelWithParent>{
            hestia::CrudMethod::CREATE,
            model,
            {},
            hestia::CrudQuery::OutputFormat::ITEM});
    REQUIRE(child_response->ok());

    hestia::CrudQuery query(
        hestia::CrudIdentifier(response->get_item()->id()),
        hestia::CrudQuery::OutputFormat::ITEM);
    auto updated_parent_response =
        m_parent_service->make_request(hestia::CrudRequest(query, {}));
    REQUIRE(updated_parent_response->ok());
    auto updated_parent =
        updated_parent_response->get_item_as<hestia::mock::MockParentModel>();
    REQUIRE(updated_parent->get_models().size() == 1);

    hestia::CrudQuery many_many_query(
        hestia::CrudIdentifier(many_target_response->get_item()->id()),
        hestia::CrudQuery::OutputFormat::ITEM);
    auto updated_many_many_response = m_many_many_parent_service->make_request(
        hestia::CrudRequest(many_many_query, {}));
    REQUIRE(updated_many_many_response->ok());
    auto updated_many_many =
        updated_many_many_response
            ->get_item_as<hestia::mock::MockManyToManyTargetModel>();
    REQUIRE(updated_many_many->get_many_to_many_children().size() == 1);
}