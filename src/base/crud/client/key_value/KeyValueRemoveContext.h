#pragma once

#include "CrudRequest.h"
#include "KeyValueFieldContext.h"

namespace hestia {
class KeyValueRemoveContext : public KeyValueFieldContext {
  public:
    KeyValueRemoveContext(
        const CrudSerializer* serializer, const std::string& key_prefix);

    void serialize_request(const CrudRequest& request);

    void prepare_db_query(std::vector<KeyValuePair>& db_query) const;

    const std::vector<std::string>& get_index_ids() const
    {
        return m_index_ids;
    }

    const std::vector<std::string>& get_index_keys() const
    {
        return m_index_keys;
    }

  private:
    std::vector<std::string> m_index_ids;
    std::vector<std::string> m_index_keys;
};
}  // namespace hestia