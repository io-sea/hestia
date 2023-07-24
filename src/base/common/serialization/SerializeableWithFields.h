#pragma once

#include "DictField.h"
#include "ScalarField.h"
#include "Serializeable.h"

namespace hestia {

/**
 * @brief An Item with Fields that can be directly converted to and from a Dictionary
 *
 * The Fields must be registered (usually in the constructor) for them to be
 * included in the serialization/deserialization. There are three types:
 *
 * - Scalar: single valued Fields such as String, Boolean types
 * - Sequence: a flat container (list) of Field items
 * - Map: can be used to represent a custom Class as a Field.
 *
 * Items can optionally have an automatically handled Uuid field.
 */

class SerializeableWithFields : public Serializeable {
  public:
    explicit SerializeableWithFields(
        const std::string& type, bool use_id = false);

    explicit SerializeableWithFields(
        const std::string& id, const std::string& type);

    SerializeableWithFields(const SerializeableWithFields& other);

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

    void get_index_fields(std::vector<KeyValuePair>& fields) const;

    std::string get_primary_key() const;

    std::string get_primary_key_name() const;

    void set_primary_key(const std::string& key);

    const std::string& id() const;

    void init_id(const std::string& id);

    bool modified() const;

    void reset();

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    SerializeableWithFields& operator=(const SerializeableWithFields& other);

  protected:
    void init();

    void register_scalar_field(ScalarField* field);

    void register_sequence_field(DictField* field);

    void register_map_field(DictField* field);

    std::vector<std::string> m_updated_fields;
    ScalarField* m_primary_key{nullptr};
    std::unordered_map<std::string, ScalarField*> m_scalar_fields;
    std::unordered_map<std::string, DictField*> m_map_fields;
    std::unordered_map<std::string, DictField*> m_sequence_fields;

    bool m_use_id{false};
    StringField m_id{"id"};
    StringField m_type{"type"};
};
}  // namespace hestia