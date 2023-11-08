#include "KeyValueRemoveContext.h"

namespace hestia {
KeyValueRemoveContext::KeyValueRemoveContext(
    const CrudSerializer* serializer, const std::string& key_prefix) :
    KeyValueFieldContext(serializer, key_prefix)
{
}

void KeyValueRemoveContext::serialize_request(const CrudRequest& request)
{
    for (const auto& id : request.get_ids().data()) {
        if (id.has_primary_key()) {
            m_index_ids.push_back(id.get_primary_key());
        }
    }

    for (const auto& id : m_index_ids) {
        m_index_keys.push_back(get_item_key(id));
    }
}

std::string KeyValueRemoveContext::get_foreign_key(
    const Model::ForeignKeyContext& foreign_key_context) const
{
    return m_key_prefix + ":" + foreign_key_context.m_type + ":"
           + foreign_key_context.m_id + ":" + m_serializer->get_type() + "s";
}

void KeyValueRemoveContext::prepare_db_query(
    std::vector<KeyValuePair>& db_query,
    const std::vector<Model::VecForeignKeyContext>& parent_refs) const
{
    for (const auto& id : m_index_ids) {
        db_query.push_back({get_set_key(), id});
    }

    std::size_t count{0};
    for (const auto& item_parent_refs : parent_refs) {
        const auto item_id = m_index_ids[count];
        for (const auto& parent_ref : item_parent_refs) {
            db_query.push_back({get_foreign_key(parent_ref), item_id});
        }
        count++;
    }
}
}  // namespace hestia