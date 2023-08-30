#include "KeyValueReadContext.h"

#include <iostream>

namespace hestia {

KeyValueReadContext::KeyValueReadContext(
    const AdapterCollection* adapters,
    const std::string& key_prefix,
    dbGetItemFunc db_get_item_func,
    dbGetSetsFunc db_get_sets_func,
    idFromParentIdFunc id_from_parent_id_func) :
    KeyValueFieldContext(adapters, key_prefix),
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
    auto template_item = m_adapters->get_model_factory()->create();
    template_item->get_foreign_key_proxy_fields(m_foreign_key_proxies);

    const auto& query = request.get_query();
    if (query.is_id()) {
        if (!serialize_ids(query, request.get_user_context())) {
            return false;
        }
    }
    else {
        if (query.get_filter().empty()) {
            serialize_empty();
        }
        else {
            if (!serialize_filter(query)) {
                return false;
            }
        }
    }
    return true;
}

bool KeyValueReadContext::serialize_ids(
    const CrudQuery& query, const CrudUserContext& user_context)
{
    auto item_template = m_adapters->get_model_factory()->create();

    for (const auto& id : query.ids()) {
        std::string working_id;
        if (id.has_primary_key()) {
            working_id = id.get_primary_key();
        }
        else if (id.has_name()) {
            working_id = get_id_from_name(id);
        }
        else if (id.has_parent_primary_key()) {
            working_id = m_id_from_parent_id_func(
                item_template->get_parent_type(), m_adapters->get_type(),
                id.get_parent_primary_key(), user_context);
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

void KeyValueReadContext::on_empty_read(
    const CrudQuery& query, CrudResponse& crud_response) const
{
    if (query.is_dict_output_format()) {
        auto response_dict = std::make_unique<Dictionary>();
        if (!query.expects_single_item()) {
            response_dict->set_type(Dictionary::Type::SEQUENCE);
        }
        crud_response.set_dict(std::move(response_dict));
    }
}

void KeyValueReadContext::process_foreign_key_content(
    const std::vector<std::string>& db_values,
    const std::vector<std::size_t>& sizes,
    Dictionary& foreign_key_dicts) const
{
    if (db_values.empty()) {
        return;
    }

    const auto adapter = m_adapters->get_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::JSON));

    std::size_t offset = 0;
    for (std::size_t idx = 0; idx < m_index_keys.size(); idx++) {
        auto item_dict = std::make_unique<Dictionary>();

        const auto num_proxies = m_foreign_key_proxies.size();

        for (std::size_t jdx = 0; jdx < num_proxies; jdx++) {
            const auto name  = m_foreign_key_proxies[jdx].second;
            const auto index = idx * num_proxies + jdx;

            add_db_items_to_dict(
                db_values, name, offset, sizes[index], adapter, *item_dict);
            offset += sizes[index];
        }
        foreign_key_dicts.add_sequence_item(std::move(item_dict));
    }
}

void KeyValueReadContext::add_db_items_to_dict(
    const std::vector<std::string>& db_items,
    const std::string& name,
    std::size_t offset,
    std::size_t size,
    const StringAdapter* adapter,
    Dictionary& dict) const
{
    assert(offset + size <= db_items.size());

    auto items_dict = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (std::size_t idx = 0; idx < size; idx++) {
        add_db_item_to_dict(db_items[offset + idx], adapter, *items_dict);
    }
    dict.set_map_item(name, std::move(items_dict));
}

void KeyValueReadContext::add_item_id(const std::string& item_id)
{
    m_index_keys.push_back(get_item_key(item_id));
    update_foreign_proxy_keys(item_id);
}

void KeyValueReadContext::add_db_item_to_dict(
    const std::string& db_item,
    const StringAdapter* adapter,
    Dictionary& dict) const
{
    auto item_dict = std::make_unique<Dictionary>();
    adapter->dict_from_string(db_item, *item_dict);
    dict.add_sequence_item(std::move(item_dict));
}

void KeyValueReadContext::update_foreign_proxy_keys(const std::string& item_id)
{
    for (const auto& [type, name] : m_foreign_key_proxies) {
        const auto key =
            get_proxy_key(m_adapters->get_type(), item_id) + ":" + type + "s";
        m_foreign_key_proxy_keys.push_back(key);
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

void KeyValueReadContext::merge_foreign_key_content(
    const Dictionary& foreign_key_dict, Dictionary& read_result) const
{
    if (foreign_key_dict.is_empty()) {
        return;
    }
    assert(!foreign_key_dict.get_sequence().empty());

    if (read_result.get_type() == Dictionary::Type::SEQUENCE) {
        assert(
            read_result.get_sequence().size()
            == foreign_key_dict.get_sequence().size());
        for (std::size_t idx = 0; idx < read_result.get_sequence().size();
             idx++) {
            read_result.get_sequence()[idx]->merge(
                *foreign_key_dict.get_sequence()[idx]);
        }
    }
    else {
        read_result.merge(*foreign_key_dict.get_sequence()[0]);
    }
}

}  // namespace hestia