#pragma once

#include "KeyValueStoreClient.h"
#include "Service.h"

namespace hestia {
struct KeyValueCrudServiceConfig {
    std::string m_endpoint;
    std::string m_prefix{"kv_crud_service"};
    std::string m_item_prefix{"item"};
};

template<typename ItemT>
class KeyValueCrudService : public CrudService<ItemT> {
  public:
    KeyValueCrudService(KeyValueStoreClient* client) :
        CrudService<ItemT>(), m_client(client){};

    void intialize(const KeyValueCrudServiceConfig& config)
    {
        m_config = config;
    }

    virtual ~KeyValueCrudService() = default;

  protected:
    KeyValueCrudServiceConfig m_config;

  private:
    void get(ItemT& item) const override
    {
        LOG_INFO("Doing get for item: " << get_item_key(item));

        const Metadata::Query query{get_item_key(item), ""};

        KeyValueStoreRequest request(
            KeyValueStoreRequestMethod::STRING_GET, query);
        const auto response = m_client->make_request(request);

        LOG_INFO("Response value is: " << response->get_value());

        from_string(response->get_value(), item);
    }

    void put(const ItemT& item) const override
    {
        std::string content;
        to_string(item, content);

        const Metadata::Query query{get_item_key(item), content};

        KeyValueStoreRequest request(
            KeyValueStoreRequestMethod::STRING_SET, query);
        const auto response = m_client->make_request(request);
        if (!response->ok()) {
            throw std::runtime_error(
                "Error in kv_store STRING_SET: "
                + response->get_error().to_string());
        }

        const Metadata::Query set_add_query{get_set_key(), item.id()};
        KeyValueStoreRequest set_request(
            KeyValueStoreRequestMethod::SET_ADD, set_add_query);
        const auto set_response = m_client->make_request(set_request);
        if (!set_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_ADD: "
                + response->get_error().to_string());
        }
    }

    bool exists(const ItemT& item) const override
    {
        const Metadata::Query query{get_item_key(item), ""};

        KeyValueStoreRequest request(
            KeyValueStoreRequestMethod::STRING_EXISTS, query);
        const auto response = m_client->make_request(request);
        if (!response->ok()) {
            throw std::runtime_error(
                "Error in kv_store STRING_EXISTS: "
                + response->get_error().to_string());
        }
        return response->has_key();
    }

    void remove(const ItemT& item) const override
    {
        const Metadata::Query query{get_item_key(item), ""};

        KeyValueStoreRequest string_request(
            KeyValueStoreRequestMethod::STRING_REMOVE, query);
        const auto string_response = m_client->make_request(string_request);
        if (!string_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store STRING_REMOVE: "
                + string_response->get_error().to_string());
        }

        const Metadata::Query set_remove_query{get_set_key(), item.id()};
        KeyValueStoreRequest set_request(
            KeyValueStoreRequestMethod::SET_REMOVE, set_remove_query);
        const auto set_response = m_client->make_request(set_request);
        if (!set_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_REMOVE: "
                + set_response->get_error().to_string());
        }
    }

    void list(std::vector<ItemT>& items) const override
    {
        const Metadata::Query query{get_set_key(), ""};

        KeyValueStoreRequest request(
            KeyValueStoreRequestMethod::SET_LIST, query);
        const auto response = m_client->make_request(request);
        if (!response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_LIST: "
                + response->get_error().to_string());
        }

        for (const auto& item : response->get_set_items()) {
            items.push_back(ItemT(item));
        }
    }

    virtual void to_string(const ItemT& item, std::string& output) const = 0;

    virtual void from_string(const std::string& output, ItemT& item) const = 0;

    std::string get_item_key(const ItemT& item) const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + ":"
               + item.id();
    }

    std::string get_set_key() const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + "s";
    }

    KeyValueStoreClient* m_client{nullptr};
};
}  // namespace hestia