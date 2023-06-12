#pragma once

#include "CrudClient.h"
#include "KeyValueStoreClient.h"

namespace hestia {
struct KeyValueCrudClientConfig {

    KeyValueCrudClientConfig() = default;

    KeyValueCrudClientConfig(
        const std::string& prefix,
        const std::string& item_prefix,
        const std::string& endpoint = {}) :
        m_prefix(prefix), m_item_prefix(item_prefix), m_endpoint(endpoint)
    {
    }

    std::string m_prefix{"kv_crud_client"};
    std::string m_item_prefix{"item"};
    std::string m_endpoint;
};

template<typename ItemT>
class KeyValueCrudClient : public CrudClient<ItemT> {
  public:
    KeyValueCrudClient(
        const KeyValueCrudClientConfig& config,
        std::unique_ptr<StringAdapter<ItemT>> adapter,
        KeyValueStoreClient* client) :
        CrudClient<ItemT>(std::move(adapter)),
        m_config(config),
        m_client(client){};

    virtual ~KeyValueCrudClient() = default;

  protected:
    KeyValueCrudClientConfig m_config;

  private:
    void get(ItemT& item) const override
    {
        const auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_GET, get_item_key(item),
             m_config.m_endpoint});
        if (!response->ok()) {
            throw RequestException<CrudRequestError>(
                {CrudErrorCode::ERROR,
                 "Error in kv_store STRING_GET: "
                     + response->get_error().to_string()});
        }
        CrudClient<ItemT>::from_string(response->item(), item);
    }

    void list(
        const Metadata& query, std::vector<std::string>& ids) const override
    {
        KeyValueStoreResponsePtr response;
        response = m_client->make_request(
            {KeyValueStoreRequestMethod::SET_LIST, get_set_key(),
             m_config.m_endpoint});
        if (!response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_LIST: "
                + response->get_error().to_string());
        }
        if (!query.empty()) {
            std::vector<ItemT> items;
            for (const auto& id : response->ids()) {
                items.push_back(ItemT(id));
            }
            multi_get({}, items);
            for (const auto& item : items) {
                if (this->matches_query(item, query)) {
                    ids.push_back(item.id());
                }
            }
        }
        else {
            for (const auto& id : response->ids()) {
                LOG_INFO("Adding id: " + id);
                ids.push_back(id);
            }
        }
    }

    void multi_get(
        const Metadata& query, std::vector<ItemT>& items) const override
    {
        std::vector<std::string> ids;
        list(query, ids);

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
            CrudClient<ItemT>::from_string(item_data, item);
            items.push_back(item);
        }
    }

    void put(const ItemT& item, bool do_generate_id = false) const override
    {
        std::string content;
        if (do_generate_id) {
            CrudClient<ItemT>::to_string(item, content, generate_id());
        }
        else {
            CrudClient<ItemT>::to_string(item, content);
        }

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
    }

    std::string generate_id() const override
    {
        std::vector<std::string> ids;
        list({}, ids);
        return std::to_string(ids.size());
    }

    bool exists(const std::string& id) const override
    {
        const auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_EXISTS, get_item_key(id),
             m_config.m_endpoint});
        if (!response->ok()) {
            throw std::runtime_error(
                "Error in kv_store STRING_EXISTS: "
                + response->get_error().to_string());
        }
        return response->found();
    }

    void remove(const std::string& id) const override
    {
        const auto string_response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_REMOVE, get_item_key(id),
             m_config.m_endpoint});
        if (!string_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store STRING_REMOVE: "
                + string_response->get_error().to_string());
        }

        const auto set_response = m_client->make_request(
            {KeyValueStoreRequestMethod::SET_REMOVE,
             Metadata::Query(get_set_key(), id), m_config.m_endpoint});
        if (!set_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_REMOVE: "
                + set_response->get_error().to_string());
        }
    }

    std::string get_item_key(const ItemT& item) const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + ":"
               + item.id();
    }

    std::string get_item_key(const std::string& id) const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + ":" + id;
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

    KeyValueStoreClient* m_client{nullptr};
};
}  // namespace hestia