#pragma once

#include "DictField.h"
#include "ScalarField.h"

#include <cassert>
#include <iostream>
#include <vector>

namespace hestia {

template<typename T>
class ForeignKeyField : public TypedDictField<T> {
  public:
    ForeignKeyField(const std::string& name) : TypedDictField<T>(name) {}
};

template<typename T>
class ForeignKeyProxyField : public DictField {
  public:
    ForeignKeyProxyField(const std::string& name) : DictField(name) {}

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override
    {
        for (const auto& model : m_models) {
            auto sequence_item_dict = std::make_unique<Dictionary>();
            model.serialize(*sequence_item_dict, format);
            dict.add_sequence_item(std::move(sequence_item_dict));
        }
    }

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override
    {
        assert(dict.get_type() == Dictionary::Type::SEQUENCE);
        for (const auto& dict_item : dict.get_sequence()) {
            m_models.emplace_back().deserialize(*dict_item, format);
        }
    }

    const std::vector<T>& models() const { return m_models; }

    std::string get_runtime_type() const override
    {
        return T().get_runtime_type();
    }

  protected:
    std::vector<T> m_models;
};

template<typename T>
class OneToOneProxyField : public TypedDictField<T> {
  public:
    OneToOneProxyField(const std::string& name) : TypedDictField<T>(name) {}
};

class NamedForeignKeyField : public DictField {
  public:
    NamedForeignKeyField(
        const std::string& name,
        const std::string& type,
        bool is_parent = false) :
        DictField(name), m_is_parent(is_parent)
    {
        m_type = type;
    }

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override
    {
        (void)format;
        dict.set_map({{m_uuid.get_name(), m_uuid.value_as_string()}});
    }

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override
    {
        (void)format;
        if (dict.has_map_item(m_uuid.get_name())) {
            m_uuid.value_from_string(
                dict.get_map_item(m_uuid.get_name())->get_scalar());
        }
    }
    bool is_parent() const { return m_is_parent; }

    const std::string& get_id() const { return m_uuid.get_value(); }

    void set_id(const std::string& id) { m_uuid.update_value(id); }

  private:
    bool m_is_parent{false};
    StringField m_uuid{"id"};
};

}  // namespace hestia
