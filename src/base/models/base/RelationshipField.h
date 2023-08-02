#pragma once

#include "DictField.h"
#include "ScalarField.h"

#include <cassert>
#include <vector>

namespace hestia {

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

class RelationshipField {
  public:
    RelationshipField(bool default_create = false) :
        m_default_create(default_create)
    {
    }

    bool should_default_create() const { return m_default_create; }

  private:
    bool m_default_create{false};
};

template<typename T>
class OneToOneProxyField : public RelationshipField, public TypedDictField<T> {
  public:
    OneToOneProxyField(const std::string& name, bool default_create = false) :
        RelationshipField(default_create), TypedDictField<T>(name)
    {
    }
};

class ForeignKeyField : public DictField {
  public:
    ForeignKeyField(
        const std::string& name,
        const std::string& type,
        bool is_parent = false);

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

    bool is_parent() const;

    const std::string& get_id() const;

    void set_id(const std::string& id);

  private:
    bool m_is_parent{false};
    StringField m_uuid{"id"};
};

class ManyToManyField : public DictField {
  public:
    ManyToManyField(const std::string& name, const std::string& type);

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

    const std::vector<std::string>& get_ids() const;

    void set_ids(const std::vector<std::string>& ids);

    void add_id(const std::string& id)
    {
        m_uuids.get_container_as_writeable().push_back(id);
    }

  private:
    ScalarSequenceField<std::vector<std::string>> m_uuids{"ids"};
};

}  // namespace hestia
