#pragma once

#include "BaseField.h"
#include "Map.h"
#include "Serializeable.h"

#include <cassert>
#include <iostream>
#include <type_traits>

namespace hestia {

/**
 * @brief A Field which is serialized to a general Dictionary structure, as distinct to a Scalar Field.
 *
 * The BaseDictField Field provides serialize and deserialize interfaces for
 * implementation in concrete types of Sequence and Map containers.
 */

class DictField : public Serializeable, public BaseField {
  public:
    DictField(const std::string& name) : Serializeable(), BaseField(name) {}
};

class RawDictField : public DictField {
  public:
    RawDictField(const std::string& name);

    const Dictionary& value() const;

    Dictionary& get_value_as_writeable();

    void update_value(const Dictionary& value);

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

  protected:
    Dictionary m_value;
};

/**
 * @brief A Field where a single Object is represented as a Dictionary
 */
template<typename T>
class TypedDictField : public DictField {
  public:
    TypedDictField(const std::string& name) : DictField(name) {}

    virtual ~TypedDictField() = default;

    const T& value() const { return m_value; }

    T& get_value_as_writeable()
    {
        m_modified = true;
        return m_value;
    }

    void update_value(const T& value)
    {
        m_value    = value;
        m_modified = true;
    }

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override
    {
        m_value.serialize(dict, format);
    }

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override
    {
        m_value.deserialize(dict, format);
    }

    std::string get_runtime_type() const override
    {
        return m_value.get_runtime_type();
    }

  protected:
    T m_value;
};

/**
 * @brief A Field with multiple items.
 */
template<typename T>
class ScalarSequenceField : public DictField {
  public:
    ScalarSequenceField(const std::string& name) : DictField(name) {}

    virtual ~ScalarSequenceField() = default;

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override
    {
        (void)format;
        for (const auto& item : m_container) {
            auto sequence_item_dict =
                std::make_unique<Dictionary>(Dictionary::Type::SCALAR);
            sequence_item_dict->set_scalar(
                item, Dictionary::ScalarType::STRING);
            dict.add_sequence_item(std::move(sequence_item_dict));
        }
    }

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override
    {
        (void)format;
        for (const auto& dict_item : dict.get_sequence()) {

            m_container.emplace_back(dict_item->get_scalar());
        }
    }

    const T& container() const { return m_container; }

    T& get_container_as_writeable()
    {
        m_modified = true;
        return m_container;
    }

  protected:
    T m_container;
};

/**
 * @brief A Field with multiple items.
 */
template<typename T>
class SequenceField : public DictField {
  public:
    SequenceField(const std::string& name) : DictField(name) {}

    virtual ~SequenceField() = default;

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override
    {
        for (const auto& item : m_container) {
            auto sequence_item_dict = std::make_unique<Dictionary>();
            item.serialize(*sequence_item_dict, format);
            dict.add_sequence_item(std::move(sequence_item_dict));
        }
    }

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override
    {
        for (const auto& dict_item : dict.get_sequence()) {
            m_container.emplace_back().deserialize(*dict_item, format);
        }
    }

    const T& container() const { return m_container; }

    T& get_container_as_writeable()
    {
        m_modified = true;
        return m_container;
    }

    std::string get_runtime_type() const override
    {
        typename T::value_type inst;
        return "seq_" + inst.get_runtime_type();
    }

  protected:
    T m_container;
};

/**
 * @brief A Field with multiple items - serialized to a Sequence Dict type, but
 * deserialized to a keyed container.
 */

template<typename T>
class BaseKeyedSequenceField : public DictField {
  public:
    BaseKeyedSequenceField(const std::string& name, const std::string& key) :
        DictField(name), m_key(key)
    {
    }

    virtual ~BaseKeyedSequenceField() = default;

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override
    {
        for (const auto& item : m_container) {
            auto sequence_item_dict = std::make_unique<Dictionary>();
            item.second.serialize(*sequence_item_dict, format);
            dict.add_sequence_item(std::move(sequence_item_dict));
        }
    }

    using IterT = decltype(std::begin(std::declval<T&>()));
    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override
    {
        assert(dict.get_type() == Dictionary::Type::SEQUENCE);
        for (const auto& dict_item : dict.get_sequence()) {
            if (dict_item->has_map_item(m_key)) {
                const auto container_key =
                    dict_item->get_map_item(m_key)->get_scalar();
                IterT emplaced_iter;
                do_emplace(container_key, emplaced_iter);
                emplaced_iter->second.deserialize(*dict_item, format);
            }
        }
    }

    virtual void do_emplace(const std::string& key, IterT& iter_out) = 0;

    const T& container() const { return m_container; }

    T& get_container_as_writeable()
    {
        m_modified = true;
        return m_container;
    }

    std::string get_runtime_type() const override
    {
        return "seq_"
               + decltype(m_container.begin()->second)().get_runtime_type();
    }

  protected:
    std::string m_key;
    T m_container;
};

template<typename T>
class KeyedSequenceField : public BaseKeyedSequenceField<T> {
  public:
    KeyedSequenceField(const std::string& name, const std::string& key) :
        BaseKeyedSequenceField<T>(name, key)
    {
    }

    virtual ~KeyedSequenceField() = default;

    void do_emplace(
        const std::string& key,
        typename BaseKeyedSequenceField<T>::IterT& iter_out) override
    {
        const auto& [iter, existed] = this->m_container.try_emplace(key);
        iter_out                    = iter;
    }
};

template<typename T>
class IntKeyedSequenceField : public BaseKeyedSequenceField<T> {
  public:
    IntKeyedSequenceField(const std::string& name, const std::string& key) :
        BaseKeyedSequenceField<T>(name, key)
    {
    }

    void do_emplace(
        const std::string& key,
        typename BaseKeyedSequenceField<T>::IterT& iter_out) override
    {
        const auto& [iter, existed] =
            this->m_container.try_emplace(std::stoi(key));
        iter_out = iter;
    }
};

/**
 * @brief A Field serializable to a Dictionary Map with only scalar values.
 */
class ScalarMapField : public DictField {
  public:
    ScalarMapField(const std::string& name);

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

    const Map& get_map() const;

    Map& get_map_as_writeable();

    void set_map_item(const std::string& key, const std::string& value);

  private:
    Map m_scalar_map;
};
}  // namespace hestia