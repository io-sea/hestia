#pragma once

#include "Dictionary.h"
#include "JsonUtils.h"
#include "Metadata.h"
#include "Model.h"
#include "Uuid.h"

#include <string>
#include <vector>

namespace hestia {

template<typename ItemT>
class StringAdapter {
  public:
    StringAdapter(
        Model::SerializeFormat format = Model::SerializeFormat::FULL) :
        m_format(format)
    {
    }

    virtual ~StringAdapter() = default;

    virtual void to_string(
        const ItemT& item, std::string& output, const Uuid& id = {}) const
    {
        Dictionary dict;
        item.serialize(dict, m_format, id);
        dict_to_string(dict, output);
    }

    virtual void to_string(
        const std::vector<ItemT>& items, std::string& output) const
    {
        auto seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
        for (const auto& item : items) {
            auto dict = std::make_unique<Dictionary>();
            item.serialize(*dict, m_format);
            seq->add_sequence_item(std::move(dict));
        }
        dict_to_string(*seq, output);
    }

    virtual void from_string(const std::string& input, ItemT& item) const
    {
        item.deserialize(*(dict_from_string(input)), m_format);
    }

    virtual void from_string(
        const std::string& input, std::vector<ItemT>& items) const
    {
        auto dict = dict_from_string(input);
        if (dict->get_type() == Dictionary::Type::SEQUENCE) {
            for (const auto& entry : dict->get_sequence()) {
                items.emplace_back();
                items[items.size() - 1].deserialize(*entry, m_format);
            }
        }
    }

    virtual bool matches_query(const ItemT& item, const Metadata& query) const
    {
        Dictionary dict;
        item.serialize(dict);

        for (const auto& [key, value] : query.get_raw_data()) {
            if (!dict.has_key_and_value({key, value})) {
                return false;
            }
        }
        return true;
    }

    virtual Dictionary::Ptr dict_from_string(
        const std::string& input) const = 0;

    virtual void dict_to_string(
        const Dictionary& dict, std::string& output) const = 0;

    Model::SerializeFormat m_format{Model::SerializeFormat::FULL};
    std::vector<std::string> m_serializer_exclude_keys;
    std::vector<std::string> m_deserializer_exclude_keys;
};

template<typename ItemT>
class JsonAdapter : public StringAdapter<ItemT> {
  public:
    JsonAdapter(Model::SerializeFormat format = Model::SerializeFormat::FULL) :
        StringAdapter<ItemT>(format)
    {
    }

    void dict_to_string(
        const Dictionary& dict, std::string& output) const override
    {
        output = JsonUtils::to_json(dict, this->m_serializer_exclude_keys);
    }

    Dictionary::Ptr dict_from_string(const std::string& input) const override
    {
        return JsonUtils::from_json(input, this->m_deserializer_exclude_keys);
    }
};


}  // namespace hestia