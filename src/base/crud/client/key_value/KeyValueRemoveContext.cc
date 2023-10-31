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

void KeyValueRemoveContext::prepare_db_query(
    std::vector<KeyValuePair>& db_query) const
{
    for (const auto& id : m_index_ids) {
        db_query.push_back({get_set_key(), id});
    }
}
}  // namespace hestia