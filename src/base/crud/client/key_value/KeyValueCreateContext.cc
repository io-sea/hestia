#include "KeyValueCreateContext.h"

#include "ErrorUtils.h"
#include "JsonDocument.h"

#include <cassert>
#include <iostream>

namespace hestia {

KeyValueCreateContext::KeyValueCreateContext(
    const CrudSerializer* serializer,
    const std::string& key_prefix,
    onIdGenerationFunc id_func,
    defaultParentIdFunc default_parent_id_func,
    getOrCreateParentFunc get_or_create_parent_func,
    createChildFunc create_child_func) :
    KeyValueFieldContext(serializer, key_prefix),
    m_id_callback(id_func),
    m_default_parent_id_func(default_parent_id_func),
    m_get_or_create_parent_func(get_or_create_parent_func),
    m_create_child_func(create_child_func)
{
}

void KeyValueCreateContext::serialize_request(
    const CrudRequest& request,
    std::vector<std::string>& output_ids,
    Dictionary& output_content)
{
    auto num_items = request.size();
    bool empty_request{false};
    if (num_items == 0) {
        empty_request = true;
        num_items     = 1;
    }

    for (std::size_t idx = 0; idx < num_items; idx++) {
        ModelRelations relations;

        Model* working_item{nullptr};
        Model::Ptr scoped_item;

        if (request.has_items()) {
            working_item = request.items()[idx].get();
        }
        else {
            scoped_item  = m_serializer->create_template(request, idx);
            working_item = scoped_item.get();
            const auto id =
                empty_request ? CrudIdentifier() : request.get_ids().id(idx);
            if (id.has_primary_key()) {
                working_item->set_primary_key(id.get_primary_key());
            }
            if (!id.get_name().empty()) {
                working_item->set_name(id.get_name());
            }
            if (!id.get_parent_name().empty()) {
                relations.m_parent_name = id.get_parent_name();
            }
            if (!id.get_parent_primary_key().empty()) {
                relations.m_parent_id = id.get_parent_primary_key();
            }
        }

        auto id = working_item->get_primary_key();
        if (id.empty()) {
            id = m_id_callback(working_item->name());
        }

        output_ids.push_back(id);
        m_serializer->append_to_dict(*working_item, output_content);
        relations.load(*working_item);
        m_relations.push_back(relations);
    }

    process_foreign_keys(request.get_user_context());
    process_one_to_one_keys(
        output_ids, output_content, request.get_user_context());

    assert(output_ids.size() == output_content.get_sequence().size());
}

void KeyValueCreateContext::process_one_to_one_keys(
    const std::vector<std::string>& output_ids,
    Dictionary& output_content,
    const CrudUserContext& user_context)
{
    assert(m_relations.size() == output_ids.size());

    // If we have a one-to-one relationship with a 'default create' property
    // then create the other side of the relationship as a child.
    std::size_t instance_count{0};
    for (const auto& relation : m_relations) {
        for (const auto& [type, name] : relation.m_one_to_one_keys) {
            auto child_dict = m_create_child_func(
                type, output_ids[instance_count], user_context);
            output_content.get_sequence()[instance_count]->set_map_item(
                name, std::move(child_dict));
        }
        instance_count++;
    }
}

std::string KeyValueCreateContext::get_foreign_key(
    const Model::ForeignKeyContext& foreign_key_context) const
{
    return m_key_prefix + ":" + foreign_key_context.m_type + ":"
           + foreign_key_context.m_id + ":" + m_serializer->get_type() + "s";
}

void KeyValueCreateContext::prepare_db_query(
    std::vector<KeyValuePair>& string_set_kv_pairs,
    std::vector<KeyValuePair>& set_add_kv_pairs,
    std::vector<std::string>& ids,
    const Dictionary& content,
    const Dictionary& create_context_dict,
    const std::string& primary_key_name) const
{
    assert(m_relations.size() == ids.size());

    std::size_t count{0};
    for (const auto& id : ids) {
        auto item_dict = content.get_sequence()[count].get();
        item_dict->merge(create_context_dict);

        // Set (override) the primary key on the dictionary
        override_field(*item_dict, primary_key_name, id);

        const auto& relations = m_relations[count];

        // Replace any required foreign key ids
        replace_foreign_key_ids(
            *item_dict, relations.m_foreign_key_id_replacements);

        // Serialize the item for saving to the db
        string_set_kv_pairs.emplace_back(
            get_item_key(id), JsonDocument(*item_dict).to_string());

        // Add id to required index fields
        for (const auto& field_index : relations.m_index) {
            const auto field_key = get_index_field_key(
                field_index, m_relations[count].m_parent_id);
            string_set_kv_pairs.emplace_back(field_key, id);
        }

        // Add id to own key set
        set_add_kv_pairs.emplace_back(get_set_key(), id);

        // Add id to any foreign key sets
        for (const auto& field_context : relations.m_foreign_keys) {
            set_add_kv_pairs.emplace_back(get_foreign_key(field_context), id);
        }

        // Add id to many-to-many key sets
        for (const auto& [type, ids] : relations.m_many_many_keys) {
            for (const auto& item_id : ids) {
                const auto many_many_key = m_key_prefix + ":" + type + ":"
                                           + item_id + ":"
                                           + m_serializer->get_type() + "s";
                set_add_kv_pairs.emplace_back(many_many_key, id);
            }
        }
        count++;
    }
}

void KeyValueCreateContext::override_field(
    Dictionary& item_dict,
    const std::string& name,
    const std::string& value) const
{
    auto key_dict = std::make_unique<Dictionary>(Dictionary::Type::SCALAR);
    key_dict->set_scalar(value, Dictionary::ScalarType::STRING);
    item_dict.set_map_item(name, std::move(key_dict));
}

std::string KeyValueCreateContext::get_index_field_key(
    const SerializeableWithFields::IndexField& index_field,
    const std::string& parent_id) const
{
    const auto& [scope, name, value] = index_field;
    std::string field_key;
    if (scope == BaseField::IndexScope::GLOBAL) {
        field_key = name;
    }
    else {
        field_key = parent_id.empty() ? name : parent_id + "::" + name;
    }
    return get_field_key(field_key, value);
}

void KeyValueCreateContext::replace_foreign_key_ids(
    Dictionary& item_dict, const VecKeyValuePair& replacements) const
{
    for (const auto& [name, id] : replacements) {
        item_dict.get_map_item(name)->set_map({{"id", id}});
    }
}

void KeyValueCreateContext::process_foreign_keys(
    const CrudUserContext& user_context)
{
    // Loop through foreign keys - if it has an id check that it exists.
    // If it has no id, but we have a parent id try to use that.
    // If it still has no id, but we have a parent name try to use that.
    // If it stillll has no id try to create a default entry.
    // If it can't create a default entry then fail.

    for (auto& relations : m_relations) {
        for (auto& field_context : relations.m_foreign_keys) {

            if (!field_context.m_id.empty()) {
                continue;
            }

            const auto parent_type =
                m_serializer->get_template()->get_parent_type();
            if (parent_type.empty()) {
                THROW_WITH_SOURCE_LOC(
                    "Have empty foreign key id and missing parent type");
            }

            if (!relations.m_parent_id.empty()) {
                field_context.m_id = relations.m_parent_id;
                relations.m_foreign_key_id_replacements.push_back(
                    {field_context.m_name, relations.m_parent_id});
            }
            else {
                auto default_parent_id = m_default_parent_id_func(parent_type);
                if (default_parent_id.empty()) {
                    m_get_or_create_parent_func(parent_type, user_context.m_id);
                    default_parent_id = m_default_parent_id_func(parent_type);
                }
                field_context.m_id = default_parent_id;
                relations.m_foreign_key_id_replacements.push_back(
                    {field_context.m_name, default_parent_id});
                relations.m_parent_id = default_parent_id;
            }
        }
    }
}


}  // namespace hestia