#include <catch2/catch_all.hpp>

#include "HsmNodeAdapter.h"
#include "HttpCrudClient.h"
#include "HttpRequest.h"
#include "StringUtils.h"

#include <iostream>
#include <unordered_map>

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
                auto iter     = m_nodes.find(id);
                if (iter == m_nodes.end()) {
                    return hestia::HttpResponse::create(404, "Not Found");
                }
                std::string body;
                m_adapter.to_string(iter->second, body);
                response->set_body(body);
            }
        }
        else if (request.get_method() == hestia::HttpRequest::Method::PUT) {
            hestia::HsmNode node;
            const auto id = hestia::StringUtils::remove_prefix(path, "/");
            m_adapter.from_string(request.body(), node);
            node.m_id   = id;
            m_nodes[id] = node;
        }
        return response;
    }

    hestia::HsmNodeJsonAdapter m_adapter;
    std::unordered_map<std::string, hestia::HsmNode> m_nodes;
};

class TestHttpCrudClientFixture {
  public:
    TestHttpCrudClientFixture()
    {
        hestia::HttpCrudClientConfig config;
        config.m_endpoint    = "127.0.0.1/api/v2/abc";
        config.m_item_prefix = "my_item_type";

        m_http_client = std::make_unique<MockHttpClient>();

        auto adapter = std::make_unique<hestia::HsmNodeJsonAdapter>();
        m_client = std::make_unique<hestia::HttpCrudClient<hestia::HsmNode>>(
            config, std::move(adapter), m_http_client.get());
    }

    std::unique_ptr<MockHttpClient> m_http_client;
    std::unique_ptr<hestia::HttpCrudClient<hestia::HsmNode>> m_client;
};

TEST_CASE_METHOD(TestHttpCrudClientFixture, "Test HttpCrudClient", "[protocol]")
{
    hestia::HsmNode node("1234");
    node.m_tag = "my_node";

    m_client->put(node, false);

    REQUIRE(m_http_client->m_nodes["1234"].id() == "1234");
    REQUIRE(m_http_client->m_nodes["1234"].m_tag == "my_node");

    hestia::HsmNode fetched_node("1234");
    REQUIRE(m_client->exists(fetched_node.id()));

    m_client->get(fetched_node);
    REQUIRE(fetched_node.id() == "1234");
    REQUIRE(fetched_node.m_tag == "my_node");

    hestia::HsmNode node1("5678");
    m_client->put(node1, false);

    std::vector<hestia::HsmNode> multi_node;
    m_client->multi_get({}, multi_node);
    REQUIRE(multi_node.size() == 2);

    std::vector<hestia::HsmNode> multi_node_query;
    hestia::Metadata query;
    query.set_item("tag", "my_node");
    m_client->multi_get(query, multi_node_query);
    REQUIRE(multi_node_query.size() == 1);
}