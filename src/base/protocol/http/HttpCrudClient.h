#pragma once

#include "CrudClient.h"
#include "HttpClient.h"
#include "RequestError.h"
#include "RequestException.h"

#include <sstream>

namespace hestia {
struct HttpCrudClientConfig {

    HttpCrudClientConfig() = default;

    HttpCrudClientConfig(
        const std::string& prefix,
        const std::string& item_prefix,
        const std::string& endpoint = {}) :
        m_prefix(prefix), m_item_prefix(item_prefix), m_endpoint(endpoint)
    {
    }

    std::string m_prefix{"http_crud_client"};
    std::string m_item_prefix{"item"};
    std::string m_endpoint;
};

template<typename ItemT>
class HttpCrudClient : public CrudClient<ItemT> {
  public:
    HttpCrudClient(
        const HttpCrudClientConfig& config,
        std::unique_ptr<StringAdapter<ItemT>> adapter,
        HttpClient* client) :
        CrudClient<ItemT>(std::move(adapter)),
        m_config(config),
        m_client(client){};

    virtual ~HttpCrudClient() = default;

    std::string generate_id() const override
    {
        throw std::runtime_error(
            "Id generation not supported for http client - the server should do this.");
    }

    bool exists(const std::string& id) const override
    {
        const auto path =
            m_config.m_endpoint + "/" + m_config.m_item_prefix + "s/" + id;
        HttpRequest request(path, HttpRequest::Method::GET);
        const auto response = m_client->make_request(request);

        return !response->error();
    }

    void get(ItemT& item) const override
    {
        const auto path = m_config.m_endpoint + "/" + m_config.m_item_prefix
                          + "s/" + item.id();
        HttpRequest request(path, HttpRequest::Method::GET);

        const auto response = m_client->make_request(request);
        if (response->error()) {
            throw RequestException<CrudRequestError>(
                {CrudErrorCode::ERROR,
                 "Error in http client GET: " + response->to_string()});
        }

        this->from_string(response->body(), item);
    }

    void multi_get(
        const Metadata& query, std::vector<ItemT>& items) const override
    {
        auto path = m_config.m_endpoint + "/" + m_config.m_item_prefix + "s";
        HttpRequest request(path, HttpRequest::Method::GET);

        if (!query.empty()) {
            request.set_queries(query);
        }

        const auto response = m_client->make_request(request);
        if (response->error()) {
            throw RequestException<CrudRequestError>(
                {CrudErrorCode::ERROR,
                 "Error in http client GET: " + response->to_string()});
        }
        this->from_string(response->body(), items);
    }

    void put(const ItemT& item, bool do_generate_id) const override
    {
        (void)do_generate_id;

        auto path = m_config.m_endpoint + "/" + m_config.m_item_prefix + "s";
        if (!item.id().empty()) {
            path += "/" + item.id();
        }

        HttpRequest request(path, HttpRequest::Method::PUT);
        this->to_string(item, request.body());

        const auto response = m_client->make_request(request);
        if (response->error()) {
            throw RequestException<CrudRequestError>(
                {CrudErrorCode::ERROR,
                 "Error in http client PUT: " + response->to_string()});
        }
    }

    void remove(const std::string& id) const override
    {
        (void)id;
        /*
        const auto string_response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_REMOVE, get_item_key(item),
             m_config.m_endpoint});
        if (!string_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store STRING_REMOVE: "
                + string_response->get_error().to_string());
        }

        const auto set_response = m_client->make_request(
            {KeyValueStoreRequestMethod::SET_REMOVE,
             Metadata::Query(get_set_key(), item.id()), m_config.m_endpoint});
        if (!set_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_REMOVE: "
                + set_response->get_error().to_string());
        }
        */
    }

    void list(
        const Metadata& query, std::vector<std::string>& ids) const override
    {
        (void)query;
        (void)ids;
        /*
        LOG_INFO("Have client: " << bool(m_client));
        const auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::SET_LIST, get_set_key(),
             m_config.m_endpoint});
        if (!response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_LIST: "
                + response->get_error().to_string());
        }

        for (const auto& id : response->ids()) {
            LOG_INFO("Adding id: " + id);
            ids.push_back(id);
        }
        */
    }

    std::string get_item_path(const ItemT& item) const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + ":"
               + item.id();
    }

    void get_item_keys(
        const std::vector<std::string>& ids,
        std::vector<std::string>& keys) const
    {
        for (const auto& id : ids) {
            keys.push_back(
                m_config.m_prefix + ":" + m_config.m_item_prefix + ":" + id);
        }
    }

    std::string get_set_key() const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + "s";
    }

  protected:
    HttpCrudClientConfig m_config;
    HttpClient* m_client{nullptr};
};
}  // namespace hestia