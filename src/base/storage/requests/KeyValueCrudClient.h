#pragma once

#include "CrudClient.h"
#include "IdGenerator.h"
#include "KeyValueStoreClient.h"
#include "UuidUtils.h"

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
        m_id_generator(std::make_unique<DefaultIdGenerator>()),
        m_config(config),
        m_client(client){};

    virtual ~KeyValueCrudClient() = default;

    std::unique_ptr<IdGenerator> m_id_generator;

  protected:
    KeyValueCrudClientConfig m_config;

  private:
    void get(ItemT& item) const override
    {
        if (item.id().is_unset()) {
            Metadata query;
            query.set_item("name", item.name());

            std::vector<ItemT> items;
            multi_get(query, items);
            if (items.empty()) {
                throw RequestException<CrudRequestError>(
                    {CrudErrorCode::ITEM_NOT_FOUND,
                     "Item with name: " + item.name() + " not found"});
            }
            item = items[0];
        }
        else {
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
    }

    void list(const Metadata& query, std::vector<Uuid>& ids) const override
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
            multi_get({}, items);
            for (const auto& item : items) {
                if (this->matches_query(item, query)) {
                    ids.push_back(item.id());
                }
            }
        }
        else {
            for (const auto& id : response->ids()) {
                ids.push_back(id);
            }
        }
    }

    void multi_get(
        const Metadata& query, std::vector<ItemT>& items) const override
    {
        std::vector<Uuid> ids;
        list(query, ids);

        if (!ids.empty()) {
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
    }

    void put(const ItemT& item, bool do_generate_id, ItemT& updated_item)
        const override
    {
        std::string content;
        Uuid working_id = item.id();
        if (working_id.is_unset() || do_generate_id) {
            working_id = generate_id(item);
            CrudClient<ItemT>::to_string(item, content, working_id);
        }
        else {
            CrudClient<ItemT>::to_string(item, content);
        }

        const auto id_str = UuidUtils::to_string(working_id);

        auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_SET,
             Metadata::Query(get_item_key(working_id), content),
             m_config.m_endpoint});
        if (!response->ok()) {
            throw RequestException<CrudRequestError>(
                {CrudErrorCode::ERROR,
                 "Error in kv_store STRING_SET: "
                     + response->get_error().to_string()});
        }

        if (!item.name().empty()) {
            auto response = m_client->make_request(
                {KeyValueStoreRequestMethod::STRING_SET,
                 Metadata::Query(get_name_key(item.name()), id_str),
                 m_config.m_endpoint});
            if (!response->ok()) {
                throw RequestException<CrudRequestError>(
                    {CrudErrorCode::ERROR,
                     "Error in kv_store STRING_SET: "
                         + response->get_error().to_string()});
            }
        }

        const auto set_response = m_client->make_request(
            {KeyValueStoreRequestMethod::SET_ADD,
             Metadata::Query(get_set_key(), id_str), m_config.m_endpoint});
        if (!set_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_ADD: "
                + response->get_error().to_string());
        }

        updated_item = item;
        updated_item.set_id(working_id);
    }

    Uuid generate_id(const ItemT& item) const override
    {
        return m_id_generator->get_uuid(
            m_config.m_prefix + "::" + m_config.m_item_prefix
            + "::" + item.name());
    }

    bool exists(const Uuid& id) const override
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

    bool exists(const std::string& name) const override
    {
        const auto response = m_client->make_request(
            {KeyValueStoreRequestMethod::STRING_GET, get_name_key(name),
             m_config.m_endpoint});
        if (!response->ok() || response->item().empty()) {
            return false;
        }
        return exists(UuidUtils::from_string(response->item()));
    }

    void remove(const Uuid& id) const override
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
             Metadata::Query(get_set_key(), UuidUtils::to_string(id)),
             m_config.m_endpoint});
        if (!set_response->ok()) {
            throw std::runtime_error(
                "Error in kv_store SET_REMOVE: "
                + set_response->get_error().to_string());
        }
    }

    std::string get_item_key(const ItemT& item) const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + ":"
               + UuidUtils::to_string(item.id());
    }

    std::string get_item_key(const Uuid& id) const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + ":"
               + UuidUtils::to_string(id);
    }

    std::string get_name_key(const std::string& name) const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix
               + "_name:" + name;
    }

    void get_item_keys(
        const std::vector<Uuid>& ids, std::vector<std::string>& keys) const
    {
        for (const auto& id : ids) {
            keys.push_back(
                m_config.m_prefix + ":" + m_config.m_item_prefix + ":"
                + UuidUtils::to_string(id));
        }
    }

    std::string get_set_key() const
    {
        return m_config.m_prefix + ":" + m_config.m_item_prefix + "s";
    }

    KeyValueStoreClient* m_client{nullptr};
};
}  // namespace hestia