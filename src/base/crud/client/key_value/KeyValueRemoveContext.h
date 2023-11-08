#pragma once

#include "CrudRequest.h"
#include "KeyValueFieldContext.h"

#include <functional>

namespace hestia {
class KeyValueRemoveContext : public KeyValueFieldContext {
  public:
    using removeChildFunc = std::function<void(
        const std::string& type,
        const std::string& child_id,
        const CrudUserContext& user_context)>;

    KeyValueRemoveContext(
        const CrudSerializer* serializer, const std::string& key_prefix);

    void serialize_request(const CrudRequest& request);

    void prepare_db_query(
        std::vector<KeyValuePair>& db_query,
        const std::vector<Model::VecForeignKeyContext>& parent_refs) const;

    const std::vector<std::string>& get_index_ids() const
    {
        return m_index_ids;
    }

    const std::vector<std::string>& get_index_keys() const
    {
        return m_index_keys;
    }

  private:
    struct ModelRelations {

        void load(const Model& model)
        {
            model.get_foreign_key_fields(m_foreign_keys);
            model.get_default_create_one_to_one_fields(m_one_to_one_keys);
            model.get_many_to_many_fields(m_many_many_keys);
        }

        Model::VecForeignKeyContext m_foreign_keys;
        VecKeyValuePair m_one_to_one_keys;
        std::vector<Model::TypeIdsPair> m_many_many_keys;
    };

    std::string get_foreign_key(
        const Model::ForeignKeyContext& foreign_key_context) const;

    removeChildFunc m_remove_child_func;
    std::vector<std::string> m_index_ids;
    std::vector<std::string> m_index_keys;
};
}  // namespace hestia