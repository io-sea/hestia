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
        const std::string&,
        const std::string&,
        const CrudUserContext& user_context)>;

    KeyValueCreateContext(
        const AdapterCollection* adapters,
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
    std::string get_foreign_key(
        const Model::ForeignKeyContext& foreign_key_context) const;

    std::string get_index_field_key(
        const SerializeableWithFields::IndexField& index_field,
        const std::vector<std::string>& parent_ids,
        std::size_t offset,
        std::size_t num_indices) const;

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

    void serialize_item(const Model& item);

    void serialize_items(
        const CrudRequest& request,
        std::vector<std::string>& output_ids,
        Dictionary& output_content);

    void serialize_ids(
        const CrudRequest& request,
        const Dictionary& input_attributes,
        std::vector<std::string>& output_ids,
        Dictionary& output_content);

    void serialize_empty(
        const Dictionary& input_attributes,
        std::vector<std::string>& output_ids,
        Dictionary& output_content);

    void to_output_dict(
        const StringAdapter& dict_adapter,
        const Model& item,
        Dictionary& output_content);

    onIdGenerationFunc m_id_callback;
    defaultParentIdFunc m_default_parent_id_func;
    getOrCreateParentFunc m_get_or_create_parent_func;
    createChildFunc m_create_child_func;

    std::vector<SerializeableWithFields::VecIndexField> m_index;
    std::vector<std::string> m_parent_names;
    std::vector<VecKeyValuePair> m_foreign_key_id_replacements;

    std::vector<Model::VecForeignKeyContext> m_foreign_key;
    std::vector<VecKeyValuePair> m_one_to_one;
    std::vector<std::vector<Model::TypeIdsPair>> m_many_many;
    std::vector<std::string> m_parent_ids;
};

}  // namespace hestia