#pragma once

#include "CrudRequest.h"
#include "KeyValueFieldContext.h"
#include "Model.h"

#include <functional>
#include <vector>

namespace hestia {

class KeyValueCreateContext : public KeyValueFieldContext {
  public:
    using onIdGenerationFunc  = std::function<std::string(const std::string&)>;
    using defaultParentIdFunc = std::function<std::string(const std::string&)>;
    using getOrCreateParentFunc =
        std::function<void(const std::string&, const std::string&)>;
    using createChildFunc = std::function<std::unique_ptr<Dictionary>(
        const std::string& type,
        const std::string& child_id,
        const CrudUserContext& user_context)>;

    KeyValueCreateContext(
        const CrudSerializer* serializer,
        const std::string& key_prefix,
        onIdGenerationFunc id_func,
        defaultParentIdFunc default_parent_id_func,
        getOrCreateParentFunc get_or_create_parent_func,
        createChildFunc create_child_func);

    void serialize_request(
        const CrudRequest& request,
        std::vector<std::string>& output_ids,
        Dictionary& output_content);

    void prepare_db_query(
        std::vector<KeyValuePair>& string_set_kv_pairs,
        std::vector<KeyValuePair>& set_add_kv_pairs,
        std::vector<std::string>& ids,
        const Dictionary& content,
        const Dictionary& create_context_dict,
        const std::string& primary_key_name) const;

  private:
    struct ModelRelations {
        void load(const Model& model)
        {
            model.get_index_fields(m_index);
            model.get_foreign_key_fields(m_foreign_keys);
            model.get_default_create_one_to_one_fields(m_one_to_one_keys);
            model.get_many_to_many_fields(m_many_many_keys);

            if (!model.get_parent_id().empty()) {
                m_parent_id = model.get_parent_id();
            }
        }

        SerializeableWithFields::VecIndexField m_index;
        std::string m_parent_id;
        std::string m_parent_name;
        VecKeyValuePair m_foreign_key_id_replacements;
        Model::VecForeignKeyContext m_foreign_keys;
        VecKeyValuePair m_one_to_one_keys;
        std::vector<Model::TypeIdsPair> m_many_many_keys;
    };

    std::string get_foreign_key(
        const Model::ForeignKeyContext& foreign_key_context) const;

    std::string get_index_field_key(
        const SerializeableWithFields::IndexField& index_field,
        const std::string& parent_id) const;

    void override_field(
        Dictionary& item_dict,
        const std::string& name,
        const std::string& value) const;

    void process_foreign_keys(const CrudUserContext& user_context);

    void process_one_to_one_keys(
        const std::vector<std::string>& output_ids,
        Dictionary& output_content,
        const CrudUserContext& user_context);

    void replace_foreign_key_ids(
        Dictionary& item_dict, const VecKeyValuePair& replacements) const;

    void serialize_item(const Model& item, ModelRelations& relations);

    onIdGenerationFunc m_id_callback;
    defaultParentIdFunc m_default_parent_id_func;
    getOrCreateParentFunc m_get_or_create_parent_func;
    createChildFunc m_create_child_func;

    std::vector<ModelRelations> m_relations;
};

}  // namespace hestia