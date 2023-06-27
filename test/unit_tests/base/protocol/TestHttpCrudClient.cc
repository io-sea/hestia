#include <catch2/catch_all.hpp>

#include "HttpCrudClient.h"
#include "HttpRequest.h"
#include "StringUtils.h"
#include "UuidUtils.h"

#include "HsmNode.h"
#include "StringAdapter.h"

#include <iostream>
#include <map>

class MockHttpClient : public hestia::HttpClient {
  public:
    hestia::HttpResponse::Ptr make_request(
        const hestia::HttpRequest& request, hestia::Stream*) override
    {
        const auto expected_prefix = "127.0.0.1/api/v2/abc/my_item_types";
        if (!hestia::StringUtils::starts_with(
                request.get_path(), expected_prefix)) {
            return hestia::HttpResponse::create(404, "Not Found");
        }

        const auto path = hestia::StringUtils::remove_prefix(
            request.get_path(), expected_prefix);
        auto response = hestia::HttpResponse::create();
        if (request.get_method() == hestia::HttpRequest::Method::GET) {
            if (path.empty()) {
                std::vector<hestia::HsmNode> matching_nodes;

                if (request.get_queries().empty()) {
                    for (const auto& [key, value] : m_nodes) {
                        matching_nodes.push_back(value);
                    }
                }
                else {
                    for (const auto& [key, value] : m_nodes) {
                        if (m_adapter.matches_query(
                                value, request.get_queries())) {
                            matching_nodes.push_back(value);
                        }
                    }
                }

                std::string body;
                m_adapter.to_string(matching_nodes, body);
                response->set_body(body);
                return response;
            }
            else {
                const auto id = hestia::StringUtils::remove_prefix(path, "/");
                auto iter = m_nodes.find(hestia::UuidUtils::from_string(id));
                if (iter == m_nodes.end()) {
                    return hestia::HttpResponse::create(404, "Not Found");
                }
                std::string body;
                m_adapter.to_string(iter->second, body);
                response->set_body(body);
            }
        }
        else if (request.get_method() == hestia::HttpRequest::Method::PUT) {

            const auto id = hestia::UuidUtils::from_string(
                hestia::StringUtils::remove_prefix(path, "/"));

            hestia::HsmNode node(id);
            m_adapter.from_string(request.body(), node);
            m_nodes[id] = node;

            std::string body;
            m_adapter.to_string(node, body);
            response->set_body(body);
        }
        return response;
    }

    hestia::JsonAdapter<hestia::HsmNode> m_adapter;
    std::map<hestia::Uuid, hestia::HsmNode> m_nodes;
};

class TestHttpCrudClientFixture {
  public:
    TestHttpCrudClientFixture()
    {
        hestia::HttpCrudClientConfig config;
        config.m_endpoint    = "127.0.0.1/api/v2/abc";
        config.m_item_prefix = "my_item_type";

        m_http_client = std::make_unique<MockHttpClient>();

        auto adapter = std::make_unique<hestia::JsonAdapter<hestia::HsmNode>>();
        m_client = std::make_unique<hestia::HttpCrudClient<hestia::HsmNode>>(
            config, std::move(adapter), m_http_client.get());
    }

    std::unique_ptr<MockHttpClient> m_http_client;
    std::unique_ptr<hestia::HttpCrudClient<hestia::HsmNode>> m_client;
};

TEST_CASE_METHOD(TestHttpCrudClientFixture, "Test HttpCrudClient", "[protocol]")
{
    hestia::Uuid id{1234};

    hestia::HsmNode node(id);
    node.set_name("my_node");

    hestia::HsmNode created_node;
    m_client->put(node, false, created_node);

    REQUIRE(m_http_client->m_nodes[id].id() == id);
    REQUIRE(m_http_client->m_nodes[id].name() == "my_node");

    return;
    REQUIRE(m_client->exists(created_node.id()));

    hestia::HsmNode fetched_node(id);
    m_client->get(fetched_node);
    REQUIRE(fetched_node.id() == id);
    REQUIRE(fetched_node.name() == "my_node");

    hestia::Uuid id1{5678};
    hestia::HsmNode node1(id1);
    m_client->put(node1, false, created_node);

    std::vector<hestia::HsmNode> multi_node;
    m_client->multi_get({}, multi_node);
    REQUIRE(multi_node.size() == 2);

    std::vector<hestia::HsmNode> multi_node_query;
    hestia::Metadata query;
    query.set_item("name", "my_node");
    m_client->multi_get(query, multi_node_query);
    REQUIRE(multi_node_query.size() == 1);
}