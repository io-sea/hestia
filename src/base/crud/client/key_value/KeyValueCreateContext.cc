#include "KeyValueCreateContext.h"

#include "ErrorUtils.h"

#include <cassert>

namespace hestia {

KeyValueCreateContext::KeyValueCreateContext(
    const AdapterCollection* adapters,
    const std::string& key_prefix,
    onIdGenerationFunc id_func,
    defaultParentIdFunc default_parent_id_func,
    getOrCreateParentFunc get_or_create_parent_func,
    createChildFunc create_child_func) :
    KeyValueFieldContext(adapters, key_prefix),
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
    Dictionary attributes;
    if (request.get_attributes().has_content()) {
        const auto adapter = m_adapters->get_adapter(
            request.get_attributes().get_format_as_string());
        adapter->dict_from_string(
            request.get_attributes().get_buffer(), attributes);
    }

    if (request.has_items()) {
        serialize_items(request, output_ids, output_content);
    }
    else if (request.get_ids().empty()) {
        serialize_empty(attributes, output_ids, output_content);
    }
    else {
        serialize_ids(request, attributes, output_ids, output_content);
        ;
    }

    process_foreign_keys(request.get_user_context());
    process_one_to_one_keys(
        output_ids, output_content, request.get_user_context());

    assert(output_ids.size() == output_content.get_sequence().size());
}

void KeyValueCreateContext::serialize_item(const Model& item)
{
    Model::VecForeignKeyContext foregin_keys;
    item.get_foreign_key_fields(foregin_keys);
    m_foreign_key.push_back(foregin_keys);

    VecKeyValuePair one_to_one_defaults;
    item.get_default_create_one_to_one_fields(one_to_one_defaults);
    m_one_to_one.push_back(one_to_one_defaults);

    std::vector<Model::TypeIdsPair> many_to_many;
    item.get_many_to_many_fields(many_to_many);
    m_many_many.push_back(many_to_many);

    if (auto parent_id = item.get_parent_id(); !parent_id.empty()) {
        m_parent_ids.push_back(parent_id);
    }
}

void KeyValueCreateContext::serialize_items(
    const CrudRequest& request,
    std::vector<std::string>& output_ids,
    Dictionary& output_content)
{
    auto dict_adapter = m_adapters->get_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::JSON));

    for (const auto& item : request.items()) {
        auto id = item->get_primary_key();
        if (id.empty()) {
            id = m_id_callback(item->name());
        }
        output_ids.push_back(id);

        SerializeableWithFields::VecIndexField index;
        item->get_index_fields(index);
        m_index.push_back(index);

        serialize_item(*item);
        to_output_dict(*dict_adapter, *item, output_content);
    }
}

void KeyValueCreateContext::serialize_ids(
    const CrudRequest& request,
    const Dictionary& input_attributes,
    std::vector<std::string>& output_ids,
    Dictionary& output_content)
{
    auto dict_adapter = m_adapters->get_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::JSON));

    std::size_t count{0};
    for (const auto& crud_id : request.get_ids()) {
        std::string id = crud_id.get_primary_key();
        if (id.empty()) {
            id = m_id_callback(crud_id.get_name());
        }
        output_ids.push_back(id);

        auto base_item = m_adapters->get_model_factory()->create();
        if (!crud_id.get_name().empty()) {
            base_item->set_name(crud_id.get_name());
        }
        if (!crud_id.get_parent_name().empty()) {
            m_parent_names.push_back(crud_id.get_parent_name());
        }
        if (!crud_id.get_parent_primary_key().empty()) {
            m_parent_ids.push_back(crud_id.get_parent_primary_key());
        }

        if (!input_attributes.is_empty()) {
            if (input_attributes.get_type() == Dictionary::Type::MAP) {
                base_item->deserialize(input_attributes);
            }
            else if (
                input_attributes.get_type() == Dictionary::Type::SEQUENCE
                && input_attributes.get_sequence().size()
                       == request.get_ids().size()) {
                base_item->deserialize(*input_attributes.get_sequence()[count]);
            }
        }

        SerializeableWithFields::VecIndexField index;
        base_item->get_index_fields(index);
        m_index.push_back(index);

        serialize_item(*base_item);
        to_output_dict(*dict_adapter, *base_item, output_content);

        count++;
    }
}

void KeyValueCreateContext::serialize_empty(
    const Dictionary& input_attributes,
    std::vector<std::string>& output_ids,
    Dictionary& output_content)
{
    auto dict_adapter = m_adapters->get_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::JSON));

    output_ids.push_back(m_id_callback(""));

    auto base_item = m_adapters->get_model_factory()->create();
    if (!input_attributes.is_empty()
        && input_attributes.get_type() == Dictionary::Type::MAP) {
        base_item->deserialize(input_attributes);
    }

    SerializeableWithFields::VecIndexField index;
    base_item->get_index_fields(index);
    m_index.push_back(index);

    serialize_item(*base_item);
    to_output_dict(*dict_adapter, *base_item, output_content);
}

std::string KeyValueCreateContext::get_foreign_key(
    const Model::ForeignKeyContext& foreign_key_context) const
{
    return m_key_prefix + ":" + foreign_key_context.m_type + ":"
           + foreign_key_context.m_id + ":" + m_adapters->get_type() + "s";
}

void KeyValueCreateContext::prepare_db_query(
    std::vector<KeyValuePair>& string_set_kv_pairs,
    std::vector<KeyValuePair>& set_add_kv_pairs,
    std::vector<std::string>& ids,
    const Dictionary& content,
    const Dictionary& create_context_dict,
    const std::string& primary_key_name) const
{
    assert(m_foreign_key_id_replacements.size() == ids.size());

    auto dict_adapter = m_adapters->get_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::JSON));

    std::size_t count{0};
    for (const auto& id : ids) {
        auto item_dict = content.get_sequence()[count].get();
        item_dict->merge(create_context_dict);

        // Set (override) the primary key on the dictionary
        override_field(*item_dict, primary_key_name, id);

        // Replace any required foreign key ids
        replace_foreign_key_ids(
            *item_dict, m_foreign_key_id_replacements[count]);

        // Serialize the item for saving to the db
        std::string content_body;
        dict_adapter->dict_to_string(*item_dict, content_body);
        string_set_kv_pairs.emplace_back(get_item_key(id), content_body);

        // Add id to required index fields
        for (const auto& field_index : m_index[count]) {
            const auto field_key = get_index_field_key(
                field_index, m_parent_ids, count, m_index.size());
            string_set_kv_pairs.emplace_back(field_key, id);
        }

        // Add id to own key set
        set_add_kv_pairs.emplace_back(get_set_key(), id);

        // Add id to any foreign key sets
        for (const auto& field_context : m_foreign_key[count]) {
            set_add_kv_pairs.emplace_back(get_foreign_key(field_context), id);
        }

        // Add id to many-to-many key sets
        for (const auto& [type, ids] : m_many_many[count]) {
            for (const auto& item_id : ids) {
                const auto many_many_key = m_key_prefix + ":" + type + ":"
                                           + item_id + ":"
                                           + m_adapters->get_type() + "s";
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
    key_dict->set_scalar(value);
    item_dict.set_map_item(name, std::move(key_dict));
}

std::string KeyValueCreateContext::get_index_field_key(
    const SerializeableWithFields::IndexField& index_field,
    const std::vector<std::string>& parent_ids,
    std::size_t offset,
    std::size_t num_indices) const
{
    const auto& [scope, name, value] = index_field;
    std::string field_key;
    if (scope == BaseField::IndexScope::GLOBAL) {
        field_key = name;
    }
    else {
        std::string parent_id;
        if (parent_ids.size() == num_indices) {
            parent_id = parent_ids[offset];
        }
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

void KeyValueCreateContext::to_output_dict(
    const StringAdapter& dict_adapter,
    const Model& item,
    Dictionary& output_content)
{
    auto item_dict = std::make_unique<Dictionary>();
    dict_adapter.to_dict(item, *item_dict);
    output_content.add_sequence_item(std::move(item_dict));
}

void KeyValueCreateContext::process_foreign_keys(
    const CrudUserContext& user_context)
{
    auto item_template = m_adapters->get_model_factory()->create();

    // Loop through foreign keys - if it has an id check that it exists.
    // If it has no id, but we have a parent id try to use that.
    // If it still has no id, but we have a parent name try to use that.
    // If it stillll has no id try to create a default entry.
    // If it can't create a default entry then fail.
    std::size_t count{0};
    for (auto& item_foreign_keys : m_foreign_key) {
        VecKeyValuePair id_replacements;
        for (auto& field_context : item_foreign_keys) {

            if (!field_context.m_id.empty()) {
                continue;
            }

            const auto parent_type = item_template->get_parent_type();
            if (parent_type.empty()) {
                THROW_WITH_SOURCE_LOC(
                    "Have empty foreign key id and missing parent type");
            }

            if (m_parent_ids.size() == m_foreign_key.size()) {
                field_context.m_id = m_parent_ids[count];
                id_replacements.push_back(
                    {field_context.m_name, m_parent_ids[count]});
            }
            else {
                auto default_parent_id = m_default_parent_id_func(parent_type);
                if (default_parent_id.empty()) {
                    m_get_or_create_parent_func(parent_type, user_context.m_id);
                    default_parent_id = m_default_parent_id_func(parent_type);
                }
                field_context.m_id = default_parent_id;
                id_replacements.push_back(
                    {field_context.m_name, default_parent_id});
                m_parent_ids.push_back(default_parent_id);
            }
        }
        m_foreign_key_id_replacements.push_back(id_replacements);
        count++;
    }
}

void KeyValueCreateContext::process_one_to_one_keys(
    const std::vector<std::string>& output_ids,
    Dictionary& output_content,
    const CrudUserContext& user_context)
{
    assert(m_one_to_one.size() == output_ids.size());

    // If we have a one-to-one relationship with a 'default create' property
    // then create the other side of the relationship as a child.
    std::size_t instance_count{0};
    for (const auto& instance : m_one_to_one) {
        for (const auto& [type, name] : instance) {
            auto child_dict = m_create_child_func(
                type, output_ids[instance_count], user_context);
            output_content.get_sequence()[instance_count]->set_map_item(
                name, std::move(child_dict));
        }
        instance_count++;
    }
}

}  // namespace hestia