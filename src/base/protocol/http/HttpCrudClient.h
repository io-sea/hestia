#pragma once

#include "CrudClient.h"
#include "HttpClient.h"

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

  protected:
    HttpCrudClientConfig m_config;

  private:
    void get(ItemT& item) const override
    {
        (void)item;
        /*
        const auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_GET, get_item_key(item),
             m_config.m_endpoint});
        if (!response->ok()) {
            throw RequestException<CrudRequestError>(
                {CrudErrorCode::ERROR,
                 "Error in kv_store STRING_GET: "
                     + response->get_error().to_string()});
        }
        from_string(response->item(), item);
        */
    }

    void multi_get(std::vector<ItemT>& items) const override
    {
        (void)items;
        /*
        std::vector<std::string> ids;
        list(ids);

        std::vector<std::string> keys;
        get_item_keys(ids, keys);

        const auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_MULTI_GET, keys,
             m_config.m_endpoint});
        if (!response->ok()) {
            throw std::runtime_error(
                "Error in kv_store STRING_MULTI_GET: "
                + response->get_error().to_string());
        }
        for (const auto& item_data : response->items()) {
            ItemT item;
            from_string(item_data, item);
            items.push_back(item);
        }
        */
    }

    void put(const ItemT& item) const override
    {
        (void)item;
        /*
        std::string content;
        to_string(item, content);

        const auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_SET,
             Metadata::Query(get_item_key(item), content),
             m_config.m_endpoint});
        if (!response->ok()) {
            throw RequestException<CrudRequestError>(
                {CrudErrorCode::ERROR,
                 "Error in kv_store STRING_SET: "
                     + response->get_error().to_string()});
        }

        const auto set_response = m_client->make_request(
            {KeyValueStoreRequestMethod::SET_ADD,
             Metadata::Query(get_set_key(), item.id()), m_config.m_endpoint});
        if (!set_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_ADD: "
                + response->get_error().to_string());
        }
        */
    }

    bool exists(const ItemT& item) const override
    {
        (void)item;
        /*
        const auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_EXISTS, get_item_key(item),
             m_config.m_endpoint});
        if (!response->ok()) {
            throw std::runtime_error(
                "Error in kv_store STRING_EXISTS: "
                + response->get_error().to_string());
        }
        return response->found();
        */
        return false;
    }

    void remove(const ItemT& item) const override
    {
        (void)item;
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

    void list(std::vector<std::string>& ids) const override
    {
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

    std::string get_item_key(const ItemT& item) const
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

    HttpClient* m_client{nullptr};
};
}  // namespace hestia