#include "KeyValueReadContext.h"

#include <iostream>

namespace hestia {

KeyValueReadContext::KeyValueReadContext(
    const CrudSerializer* serializer,
    const std::string& key_prefix,
    dbGetItemFunc db_get_item_func,
    dbGetSetsFunc db_get_sets_func,
    idFromParentIdFunc id_from_parent_id_func) :
    KeyValueFieldContext(serializer, key_prefix),
    m_db_get_item_func(db_get_item_func),
    m_db_get_sets_func(db_get_sets_func),
    m_id_from_parent_id_func(id_from_parent_id_func)
{
}

const std::vector<std::string>& KeyValueReadContext::get_index_keys() const
{
    return m_index_keys;
}

bool KeyValueReadContext::serialize_request(const CrudRequest& request)
{
    m_serializer->get_template()->get_foreign_key_proxy_fields(
        m_foreign_key_proxies);

    m_serializer->get_template()->get_one_to_one_fields(
        m_one_to_one_key_proxies);

    if (request.has_ids()) {
        if (!serialize_ids(request.get_ids(), request.get_user_context())) {
            return false;
        }
    }
    else {
        if (request.get_query().get_filter().empty()) {
            serialize_empty();
        }
        else {
            if (!serialize_filter(request.get_query())) {
                return false;
            }
        }
    }
    return true;
}

bool KeyValueReadContext::serialize_ids(
    const CrudIdentifierCollection& ids, const CrudUserContext& user_context)
{
    for (const auto& id : ids.data()) {
        std::string working_id;
        if (id.has_primary_key()) {
            working_id = id.get_primary_key();
        }
        else if (id.has_name()) {
            working_id = get_id_from_name(id);
        }
        else if (id.has_parent_primary_key()) {
            working_id = m_id_from_parent_id_func(
                m_serializer->get_template()->get_parent_type(),
                m_serializer->get_type(), id.get_parent_primary_key(),
                user_context);
        }

        if (working_id.empty()) {
            return false;
        }
        add_item_id(working_id);
    }
    return true;
}

bool KeyValueReadContext::serialize_filter(const CrudQuery& query)
{
    const auto& [key, value] = *query.get_filter().data().begin();
    const auto matching_id   = m_db_get_item_func(get_field_key(key, value));
    if (matching_id.empty()) {
        return false;
    }
    add_item_id(matching_id);
    return true;
}

void KeyValueReadContext::serialize_empty()
{
    std::vector<std::vector<std::string>> db_response;
    m_db_get_sets_func({get_set_key()}, db_response);

    if (!db_response.empty()) {
        for (const auto& id : db_response[0]) {
            add_item_id(id);
        }
    }
}

std::string KeyValueReadContext::get_id_from_name(
    const CrudIdentifier& id) const
{
    std::string field_prefix;
    if (id.has_parent_primary_key()) {
        field_prefix += id.get_parent_primary_key() + "::";
    }
    return m_db_get_item_func(
        get_field_key(field_prefix + "name", id.get_name()));
}

void KeyValueReadContext::process_foreign_key_content(
    const std::vector<std::string>& db_values,
    const std::vector<std::size_t>& sizes,
    Dictionary& foreign_key_dicts) const
{
    if (db_values.empty()) {
        return;
    }

    std::size_t offset = 0;
    for (std::size_t idx = 0; idx < m_index_keys.size(); idx++) {
        auto item_dict = std::make_unique<Dictionary>();

        const auto num_proxies = m_foreign_key_proxies.size();

        for (std::size_t jdx = 0; jdx < num_proxies; jdx++) {
            const auto name  = m_foreign_key_proxies[jdx].second;
            const auto index = idx * num_proxies + jdx;

            add_db_items_to_dict(
                db_values, name, offset, sizes[index], *item_dict);
            offset += sizes[index];
        }
        foreign_key_dicts.add_sequence_item(std::move(item_dict));
    }
}

void KeyValueReadContext::process_one_to_one_content(
    const std::vector<std::string>& db_values, Dictionary& key_dicts) const
{
    if (db_values.empty()) {
        return;
    }

    for (std::size_t idx = 0; idx < m_index_keys.size(); idx++) {
        auto item_dict         = Dictionary::create();
        const auto num_proxies = m_one_to_one_key_proxies.size();
        for (std::size_t jdx = 0; jdx < num_proxies; jdx++) {
            const auto name            = m_one_to_one_key_proxies[jdx].second;
            const auto index           = idx * num_proxies + jdx;
            auto one_to_one_entry_dict = Dictionary::create();
            JsonDocument(db_values[index]).write(*one_to_one_entry_dict);
            item_dict->set_map_item(name, std::move(one_to_one_entry_dict));
        }
        key_dicts.add_sequence_item(std::move(item_dict));
    }
}


void KeyValueReadContext::add_db_items_to_dict(
    const std::vector<std::string>& db_items,
    const std::string& name,
    std::size_t offset,
    std::size_t size,
    Dictionary& dict) const
{
    assert(offset + size <= db_items.size());

    auto items_dict = Dictionary::create(Dictionary::Type::SEQUENCE);
    for (std::size_t idx = 0; idx < size; idx++) {
        add_db_item_to_dict(db_items[offset + idx], *items_dict);
    }
    dict.set_map_item(name, std::move(items_dict));
}

void KeyValueReadContext::add_item_id(const std::string& item_id)
{
    m_index_keys.push_back(get_item_key(item_id));
    update_foreign_proxy_keys(item_id);
    update_one_to_one_keys(item_id);
}

void KeyValueReadContext::add_db_item_to_dict(
    const std::string& db_item, Dictionary& dict) const
{
    auto item_dict = Dictionary::create();
    JsonDocument(db_item).write(*item_dict);
    dict.add_sequence_item(std::move(item_dict));
}

void KeyValueReadContext::update_foreign_proxy_keys(const std::string& item_id)
{
    for (const auto& [type, name] : m_foreign_key_proxies) {
        const auto key =
            get_proxy_key(m_serializer->get_type(), item_id) + ":" + type + "s";
        m_foreign_key_proxy_keys.push_back(key);
    }
}

void KeyValueReadContext::update_one_to_one_keys(const std::string& item_id)
{
    for (const auto& [type, name] : m_one_to_one_key_proxies) {
        const auto key =
            get_proxy_key(m_serializer->get_type(), item_id) + ":" + type + "s";
        m_one_to_one_keys.push_back(key);
    }
}

std::string KeyValueReadContext::get_proxy_key(
    const std::string& type, const std::string& id) const
{
    return m_key_prefix + ":" + type + ":" + id;
}

const std::vector<std::string>&
KeyValueReadContext::get_foreign_key_proxy_keys() const
{
    return m_foreign_key_proxy_keys;
}

const std::vector<std::string>& KeyValueReadContext::get_one_to_one_keys() const
{
    return m_one_to_one_keys;
}

bool KeyValueReadContext::has_one_to_one_content() const
{
    return !m_one_to_one_keys.empty();
}

bool KeyValueReadContext::has_foreign_key_content() const
{
    return !m_foreign_key_proxy_keys.empty();
}

void KeyValueReadContext::get_foreign_key_query(
    const std::vector<std::vector<std::string>>& ids,
    std::vector<std::string>& keys,
    std::vector<std::size_t>& sizes) const
{
    const auto num_proxies = m_foreign_key_proxies.size();

    if (ids.empty()) {
        sizes.assign(m_index_keys.size() * num_proxies, 0);
        return;
    }

    for (std::size_t idx = 0; idx < m_index_keys.size(); idx++) {
        for (std::size_t jdx = 0; jdx < num_proxies; jdx++) {
            const auto offset = idx * num_proxies + jdx;
            const auto type   = m_foreign_key_proxies[jdx].first;
            assert(offset < ids.size());
            for (const auto& id : ids[offset]) {
                keys.push_back(get_proxy_key(type, id));
            }
            sizes.push_back(ids[offset].size());
        }
    }
}

void KeyValueReadContext::get_one_to_one_key_query(
    const std::vector<std::vector<std::string>>& ids,
    std::vector<std::string>& keys,
    std::vector<std::size_t>& sizes) const
{
    const auto num_proxies = m_one_to_one_key_proxies.size();

    if (ids.empty()) {
        sizes.assign(m_index_keys.size() * num_proxies, 0);
        return;
    }

    for (std::size_t idx = 0; idx < m_index_keys.size(); idx++) {
        for (std::size_t jdx = 0; jdx < num_proxies; jdx++) {
            const auto offset = idx * num_proxies + jdx;
            const auto type   = m_one_to_one_key_proxies[jdx].first;
            assert(offset < ids.size());
            for (const auto& id : ids[offset]) {
                keys.push_back(get_proxy_key(type, id));
            }
            sizes.push_back(ids[offset].size());
        }
    }
}

void KeyValueReadContext::merge_proxy_content(
    const Dictionary& key_dict, Dictionary& read_result) const
{
    if (key_dict.is_empty()) {
        return;
    }
    assert(!key_dict.get_sequence().empty());

    if (read_result.get_type() == Dictionary::Type::SEQUENCE) {
        assert(
            read_result.get_sequence().size()
            == key_dict.get_sequence().size());
        for (std::size_t idx = 0; idx < read_result.get_sequence().size();
             idx++) {
            read_result.get_sequence()[idx]->merge(
                *key_dict.get_sequence()[idx]);
        }
    }
    else {
        read_result.merge(*key_dict.get_sequence()[0]);
    }
}

}  // namespace hestia