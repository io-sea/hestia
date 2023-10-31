#include "KeyValueCrudClient.h"

#include "IdGenerator.h"
#include "KeyValueStoreClient.h"
#include "KeyValueStoreRequest.h"
#include "TimeProvider.h"

#include "CrudService.h"
#include "ErrorUtils.h"
#include "Logger.h"
#include "RequestException.h"

#include "KeyValueCreateContext.h"
#include "KeyValueReadContext.h"
#include "KeyValueRemoveContext.h"
#include "KeyValueUpdateContext.h"

#include <cassert>
#include <iostream>

namespace hestia {
KeyValueCrudClient::KeyValueCrudClient(
    const CrudClientConfig& config,
    ModelSerializer::Ptr serializer,
    KeyValueStoreClient* client,
    IdGenerator* id_generator,
    TimeProvider* time_provider) :
    CrudClient(config, std::move(serializer), id_generator, time_provider),
    m_client(client)
{
    m_serializer->load_template();
}

KeyValueCrudClient::~KeyValueCrudClient() {}

void KeyValueCrudClient::prepare_creation_overrides(
    const CrudUserContext& user_context,
    Dictionary& create_overrides_dict) const
{
    const auto current_time = m_time_provider->get_current_time();

    ModelCreationContext create_overrides(m_serializer->get_runtime_type());
    create_overrides.m_creation_time.update_value(current_time);
    create_overrides.m_last_modified_time.update_value(current_time);
    if (m_serializer->type_has_owner()) {
        create_overrides.add_user(user_context.m_id);
    }
    create_overrides.serialize(create_overrides_dict);
}

void KeyValueCrudClient::create(
    const CrudRequest& crud_request,
    CrudResponse& crud_response,
    bool record_modified_attrs)
{
    auto id_generation_func = [this](const std::string& name) {
        return generate_id(name);
    };
    auto default_parent_id_func = [this](const std::string& type) {
        return get_default_parent_id(type);
    };
    auto get_or_create_parent_func =
        [this](const std::string& type, const std::string& user_id) {
            return get_or_create_default_parent(type, user_id);
        };
    auto create_child_func = [this](
                                 const std::string& type,
                                 const std::string& parent_id,
                                 const CrudUserContext& user_context) {
        return create_child(type, parent_id, user_context);
    };

    KeyValueCreateContext create_context(
        m_serializer.get(), m_config.m_prefix, id_generation_func,
        default_parent_id_func, get_or_create_parent_func, create_child_func);

    // Convert the request to a sequence of primary-keys (ids) and corresponding
    // content bodies (as dictionary sequence entries)
    std::vector<std::string> ids;
    auto content = Dictionary::create(Dictionary::Type::SEQUENCE);
    create_context.serialize_request(crud_request, ids, *content);

    // Prepare a dictionary to override creation-related fields (e.g. created
    // and last modified times)
    Dictionary creation_overrides;
    prepare_creation_overrides(
        crud_request.get_user_context(), creation_overrides);

    // Prepare the query for the key value store
    std::vector<KeyValuePair> string_set_queries;
    std::vector<KeyValuePair> set_add_queries;
    create_context.prepare_db_query(
        string_set_queries, set_add_queries, ids, *content, creation_overrides,
        m_serializer->get_template()->get_primary_key_name());

    // Make batch requests to the STRING and SET kv store endpoints
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_SET, string_set_queries,
         m_config.m_endpoint});
    error_check("STRING_SET", response.get());

    const auto set_response = m_client->make_request(
        {KeyValueStoreRequestMethod::SET_ADD, set_add_queries,
         m_config.m_endpoint});
    error_check("SET_ADD", set_response.get());

    m_serializer->append_dict(
        std::move(content), crud_response, record_modified_attrs);
}

void KeyValueCrudClient::prepare_update_overrides(
    Dictionary& update_overrides) const
{
    const auto current_time = m_time_provider->get_current_time();
    ModelUpdateContext update_context(m_serializer->get_type());
    update_context.m_last_modified_time.update_value(current_time);
    update_context.serialize(update_overrides);
}

void KeyValueCrudClient::update(
    const CrudRequest& crud_request,
    CrudResponse& crud_response,
    bool record_modified_attrs) const
{
    // create a GET db query for each item in the request
    auto id_from_parent_id_func = [this](
                                      const std::string& parent_type,
                                      const std::string& child_type,
                                      const std::string& id,
                                      const CrudUserContext& user_context) {
        return get_id_from_parent_id(parent_type, child_type, id, user_context);
    };

    auto content = std::make_unique<Dictionary>();
    KeyValueUpdateContext update_context(
        m_serializer.get(), m_config.m_prefix, id_from_parent_id_func);
    update_context.serialize_request(crud_request, *content);

    // Get the queried items from the db.
    VecModelPtr db_items;
    get_db_items(update_context.get_index_keys(), db_items);

    // Prepare overrides for update content, e.g. last modified time
    Dictionary update_overrides;
    prepare_update_overrides(update_overrides);

    // Prepare the UPDATE db query
    auto updated_content = std::make_unique<Dictionary>();
    std::vector<KeyValuePair> string_set_query;
    update_context.prepare_db_query(
        crud_request, *content, db_items, update_overrides, *updated_content,
        string_set_query);

    // Do the db query
    const auto set_response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_SET, string_set_query,
         m_config.m_endpoint});
    error_check("STRING_SET", set_response.get());

    m_serializer->append_dict(
        std::move(updated_content), crud_response, record_modified_attrs);
}

void KeyValueCrudClient::read(
    const CrudRequest& request, CrudResponse& crud_response) const
{
    // Build a db READ query from request - this may itself query the db as part
    // of the query building
    auto db_get_item_func = [this](const std::string& key) {
        return get_db_item(key);
    };
    auto db_get_sets_func =
        [this](
            const std::vector<std::string>& keys,
            std::vector<std::vector<std::string>>& response) {
            return get_db_sets(keys, response);
        };
    auto id_from_parent_id_func = [this](
                                      const std::string& parent_type,
                                      const std::string& child_type,
                                      const std::string& id,
                                      const CrudUserContext& user_context) {
        return get_id_from_parent_id(parent_type, child_type, id, user_context);
    };
    KeyValueReadContext read_context(
        m_serializer.get(), m_config.m_prefix, db_get_item_func,
        db_get_sets_func, id_from_parent_id_func);
    if (!read_context.serialize_request(request)) {

        return;
    }

    // Read item content from the DB
    auto read_content = std::make_unique<Dictionary>();
    if (!get_db_items(
            read_context.get_index_keys(), *read_content,
            request.get_query().expects_single_item())) {
        return;
    }

    // Read foreign key items
    Dictionary foreign_key_content(Dictionary::Type::SEQUENCE);
    if (read_context.has_foreign_key_content()) {
        // Build db queries for foreign key items
        std::vector<std::vector<std::string>> foreign_key_ids;
        get_db_sets(read_context.get_foreign_key_proxy_keys(), foreign_key_ids);

        std::vector<std::string> foreign_keys;
        std::vector<std::size_t> foreign_key_set_sizes;
        read_context.get_foreign_key_query(
            foreign_key_ids, foreign_keys, foreign_key_set_sizes);

        // Get the foreign key content from DB
        read_context.process_foreign_key_content(
            get_db_items(foreign_keys), foreign_key_set_sizes,
            foreign_key_content);
    }

    read_context.merge_foreign_key_content(foreign_key_content, *read_content);

    m_serializer->append_dict(std::move(read_content), crud_response);
}

void KeyValueCrudClient::remove(
    const CrudRequest& request, CrudResponse& crud_response) const
{
    // Prepare a db GET query
    KeyValueRemoveContext remove_context(m_serializer.get(), m_config.m_prefix);
    remove_context.serialize_request(request);

    // Do the query - if any item not found then fail
    const auto db_items = get_db_items(remove_context.get_index_keys());

    if (db_items.empty()) {
        LOG_INFO("Failed to find requested key - not attempting removal");
        return;
    }
    const auto any_empty = std::any_of(
        db_items.begin(), db_items.end(),
        [](const std::string& entry) { return entry.empty(); });
    if (any_empty) {
        LOG_INFO("Failed to find requested key - not attempting removal");
        return;
    }

    // Set up the db removal queries
    std::vector<KeyValuePair> set_remove_keys;
    remove_context.prepare_db_query(set_remove_keys);

    // Do the db removal
    const auto string_response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_REMOVE,
         remove_context.get_index_keys(), m_config.m_endpoint});
    error_check("STRING_REMOVE", string_response.get());

    const auto set_response = m_client->make_request(
        {KeyValueStoreRequestMethod::SET_REMOVE, set_remove_keys,
         m_config.m_endpoint});
    error_check("SET_REMOVE", string_response.get());

    // Prepare the response
    crud_response.ids() = remove_context.get_index_ids();
}

void KeyValueCrudClient::assign_modified_attributes(
    const Dictionary& content, CrudResponse& response) const
{
    if (content.get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& item : content.get_sequence()) {
            Map flat_attrs;
            item->flatten(flat_attrs);
            response.modified_attrs().push_back(flat_attrs);
        }
    }
    else {
        Map flat_attrs;
        content.flatten(flat_attrs);
        response.modified_attrs().push_back(flat_attrs);
    }
}

std::string KeyValueCrudClient::get_db_item(const std::string& key) const
{
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_GET, {key}, m_config.m_endpoint});
    error_check("STRING_GET", response.get());
    if (response->items().empty() || response->items()[0].empty()) {
        return {};
    }
    return response->items()[0];
}

std::vector<std::string> KeyValueCrudClient::get_db_items(
    const std::vector<std::string>& keys) const
{
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_GET, keys, m_config.m_endpoint});
    error_check("GET", response.get());
    return response->get_items();
}

void KeyValueCrudClient::get_db_items(
    const std::vector<std::string>& keys, VecModelPtr& items) const
{
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_GET, keys, m_config.m_endpoint});
    error_check("GET", response.get());

    const auto any_false = std::any_of(
        response->found().begin(), response->found().end(),
        [](bool v) { return !v; });
    if (any_false) {
        throw std::runtime_error("Attempted to get a non-existing resource");
    }

    Dictionary dict;
    JsonDocument(response->items()).write(dict);
    m_serializer->get_model_serializer()->from_dict(dict, items);
}

bool KeyValueCrudClient::get_db_items(
    const std::vector<std::string>& keys,
    Dictionary& db_content,
    bool expects_single) const
{
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_GET, keys, m_config.m_endpoint});
    error_check("GET", response.get());
    if (response->items().empty()) {
        return false;
    }

    const auto any_empty = std::any_of(
        response->items().begin(), response->items().end(),
        [](const std::string& entry) { return entry.empty(); });
    if (any_empty) {
        return false;
    }

    (void)expects_single;
    JsonDocument(response->items()).write(db_content);
    return true;
}

void KeyValueCrudClient::get_db_sets(
    const std::vector<std::string>& keys,
    std::vector<std::vector<std::string>>& values) const
{
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::SET_LIST, keys, m_config.m_endpoint});
    error_check("SET LIST", response.get());
    values = response->ids();
}

void KeyValueCrudClient::identify(
    const CrudRequest& request, CrudResponse& response) const
{
    (void)request;
    (void)response;
}

void KeyValueCrudClient::lock(
    const CrudIdentifier& id, CrudLockType lock_type) const
{
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_SET,
         {KeyValuePair(get_lock_key(id.get_primary_key(), lock_type), "1")},
         m_config.m_endpoint});
    error_check("STRING_SET", response.get());
}

void KeyValueCrudClient::unlock(
    const CrudIdentifier& id, CrudLockType lock_type) const
{
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_REMOVE,
         {get_lock_key(id.get_primary_key(), lock_type)},
         m_config.m_endpoint});
    error_check("STRING_REMOVE", response.get());
}

bool KeyValueCrudClient::is_locked(
    const CrudIdentifier& id, CrudLockType lock_type) const
{
    const auto response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_EXISTS,
         {get_lock_key(id.get_primary_key(), lock_type)},
         m_config.m_endpoint});

    error_check("STRING_EXISTS", response.get());
    return response->found()[0];
}

std::string KeyValueCrudClient::get_lock_key(
    const std::string& id, CrudLockType lock_type) const
{
    std::string lock_str = lock_type == CrudLockType::READ ? "r" : "w";
    return m_config.m_prefix + ":" + m_serializer->get_type() + "_lock"
           + lock_str + ":" + id;
}

void KeyValueCrudClient::error_check(
    const std::string& identifier, const BaseResponse* response) const
{
    if (!response->ok()) {
        const std::string msg = "Error in kv_store " + identifier + ": "
                                + response->get_base_error().to_string();
        LOG_ERROR(msg);
        throw RequestException<CrudRequestError>({CrudErrorCode::ERROR, msg});
    }
}

}  // namespace hestia