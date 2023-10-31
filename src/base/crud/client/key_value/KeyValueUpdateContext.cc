#include "KeyValueUpdateContext.h"

#include "Logger.h"
#include <iostream>

namespace hestia {

KeyValueUpdateContext::KeyValueUpdateContext(
    const CrudSerializer* serializer,
    const std::string& key_prefix,
    idFromParentIdFunc id_from_parent_id_func) :
    KeyValueFieldContext(serializer, key_prefix),
    m_id_from_parent_id_func(id_from_parent_id_func)
{
}

void KeyValueUpdateContext::serialize_request(
    const CrudRequest& request, Dictionary& output_content)
{
    if (request.has_items()) {
        for (const auto& item : request.items()) {
            m_index_ids.push_back(item->get_primary_key());
        }
    }
    else {
        auto item_template = m_serializer->create_template();
        for (const auto& id : request.get_ids().data()) {
            if (id.has_primary_key()) {
                m_index_ids.push_back(id.get_primary_key());
            }
            else if (id.has_parent_primary_key()) {
                const auto id_from_parent = m_id_from_parent_id_func(
                    item_template->get_parent_type(), m_serializer->get_type(),
                    id.get_parent_primary_key(), request.get_user_context());
                if (id_from_parent.empty()) {
                    throw std::runtime_error(
                        "Failed to find item id for parent id: "
                        + id.get_parent_primary_key());
                }
                m_index_ids.push_back(id_from_parent);
            }
        }

        std::vector<std::string> ids;
        request.get_attributes().get_values(SearchExpression("id"), ids);
        for (const auto& id : ids) {
            m_index_ids.push_back(id);
        }

        if (request.get_attributes().has_content()) {
            output_content.merge(*request.get_attributes().get_copy_as_dict());
        }
    }
    for (const auto& id : m_index_ids) {
        m_index_keys.push_back(get_item_key(id));
    }
}

void KeyValueUpdateContext::prepare_db_query(
    const CrudRequest& request,
    const Dictionary& input_attributes,
    const VecModelPtr& db_items,
    const Dictionary& update_overrides,
    Dictionary& updated_content,
    std::vector<KeyValuePair>& db_query) const
{
    if (request.has_items()) {
        assert(request.items().size() == db_items.size());

        std::size_t count = 0;
        for (auto& db_item : db_items) {
            Dictionary modified_dict;
            request.items()[count]->serialize(
                modified_dict, Serializeable::Format::MODIFIED);

            modified_dict.merge(update_overrides);
            db_item->deserialize(modified_dict);
            count++;
        }
    }
    else {
        if (input_attributes.get_type() == Dictionary::Type::SEQUENCE) {
            std::size_t count = 0;
            for (auto& db_item : db_items) {
                auto each_update_context = update_overrides;
                each_update_context.merge(
                    *input_attributes.get_sequence()[count]);
                db_item->deserialize(each_update_context);
                count++;
            }
        }
        else {
            auto each_update_context = update_overrides;
            each_update_context.merge(input_attributes);
            for (auto& db_item : db_items) {
                db_item->deserialize(each_update_context);
            }
        }
    }

    m_serializer->get_model_serializer()->to_dict(db_items, updated_content);

    prepare_query_keys(updated_content, db_query);
}

void KeyValueUpdateContext::prepare_query_keys(
    const Dictionary& updated_content,
    std::vector<KeyValuePair>& db_query) const
{
    if (updated_content.get_type() == Dictionary::Type::SEQUENCE) {
        std::size_t count = 0;
        for (const auto& dict_item : updated_content.get_sequence()) {
            db_query.push_back(
                {m_index_keys[count], JsonDocument(*dict_item).to_string()});
            count++;
        }
    }
    else {
        assert(m_index_keys.size() == 1);
        db_query.push_back(
            {m_index_keys[0], JsonDocument(updated_content).to_string()});
    }
}

}  // namespace hestia