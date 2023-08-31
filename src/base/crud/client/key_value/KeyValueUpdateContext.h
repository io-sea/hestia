#pragma once

#include "CrudRequest.h"
#include "KeyValueFieldContext.h"

#include <functional>

namespace hestia {
class KeyValueUpdateContext : public KeyValueFieldContext {
  public:
    using idFromParentIdFunc = std::function<std::string(
        const std::string&,
        const std::string&,
        const std::string&,
        const CrudUserContext&)>;

    KeyValueUpdateContext(
        const AdapterCollection* adapters,
        const std::string& key_prefix,
        idFromParentIdFunc id_from_parent_id_func);

    void serialize_request(
        const CrudRequest& request, Dictionary& output_content);

    void prepare_db_query(
        const CrudRequest& request,
        const Dictionary& input_attributes,
        const VecModelPtr& db_items,
        const Dictionary& update_overrides,
        Dictionary& updated_content,
        std::vector<KeyValuePair>& db_query) const;

    const std::vector<std::string>& get_index_ids() const
    {
        return m_index_ids;
    }

    const std::vector<std::string>& get_index_keys() const
    {
        return m_index_keys;
    }

  private:
    void prepare_query_keys(
        const Dictionary& updated_content,
        std::vector<KeyValuePair>& db_query) const;

    idFromParentIdFunc m_id_from_parent_id_func;

    std::vector<std::string> m_index_ids;
    std::vector<std::string> m_index_keys;
};
}  // namespace hestia