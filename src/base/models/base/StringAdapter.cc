#include "StringAdapter.h"

#include "JsonUtils.h"
#include "StringUtils.h"

#include <stdexcept>

namespace hestia {
StringAdapter::StringAdapter(
    ModelFactory* model_factory, Serializeable::Format format) :
    m_model_factory(model_factory), m_format(format)
{
}

StringAdapter::~StringAdapter() {}

void StringAdapter::to_string(
    const VecModelPtr& items,
    std::string& output,
    const std::vector<Dictionary>& overrides,
    int index,
    Serializeable::Format format_in) const
{
    Dictionary dict;
    to_dict(items, dict, overrides, index, format_in);
    dict_to_string(dict, output);
}

void StringAdapter::to_dict(
    const VecModelPtr& items,
    Dictionary& output,
    const std::vector<Dictionary>& overrides,
    int index,
    Serializeable::Format format_in) const
{
    const auto format =
        format_in == Serializeable::Format::UNSET ? m_format : format_in;

    if (items.empty()) {
        m_model_factory->create()->serialize(output, format);
        if (overrides.size() == 1) {
            output.merge(overrides[0]);
        }
    }
    else if (items.size() == 1) {
        items[0]->serialize(output, format);
        if (overrides.size() == 1) {
            output.merge(overrides[0]);
        }
    }
    else if (index > -1) {
        if (static_cast<std::size_t>(index) >= items.size()) {
            throw std::runtime_error(
                "Out of bounds access attempted in string adapter conversion");
        }

        items[index]->serialize(output, format);
        if (overrides.size() == items.size()) {
            output.merge(overrides[0]);
        }
    }
    else {
        output.set_type(Dictionary::Type::SEQUENCE);
        const bool use_overrides = items.size() == overrides.size();
        std::size_t count{0};
        for (const auto& item : items) {
            auto item_dict = std::make_unique<Dictionary>();
            item->serialize(*item_dict, m_format);
            if (use_overrides) {
                item_dict->merge(overrides[count]);
            }
            output.add_sequence_item(std::move(item_dict));
            count++;
        }
    }
}

void StringAdapter::from_string(
    const std::vector<std::string>& inputs,
    std::vector<Model::Ptr>& items,
    bool as_list) const
{
    Dictionary dict;
    from_string(inputs, dict, as_list);
    from_dict(dict, items);
}

void StringAdapter::from_string(
    const std::vector<std::string>& inputs,
    Dictionary& dict,
    bool as_list) const
{
    if (inputs.empty()) {
        return;
    }

    if (!as_list && inputs.size() == 1) {
        dict_from_string(inputs[0], dict);
    }
    else {
        dict.set_type(Dictionary::Type::SEQUENCE);
        for (const auto& input : inputs) {
            auto item_dict = std::make_unique<Dictionary>();
            dict_from_string(input, *item_dict);
            dict.add_sequence_item(std::move(item_dict));
        }
    }
}

void StringAdapter::from_dict(const Dictionary& dict, VecModelPtr& items) const
{
    if (dict.is_empty()) {
        return;
    }

    if (dict.get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& entry : dict.get_sequence()) {
            auto item = m_model_factory->create();
            item->deserialize(*entry, m_format);
            items.push_back(std::move(item));
        }
    }
    else {
        if (items.empty()) {
            auto item = m_model_factory->create();
            item->deserialize(dict, m_format);
            items.push_back(std::move(item));
        }
        else {
            items[0]->deserialize(dict, m_format);
        }
    }
}

bool StringAdapter::matches_query(const Model& item, const Map& query) const
{
    Dictionary dict;
    item.serialize(dict);

    for (const auto& [key, value] : query.data()) {
        if (!dict.has_key_and_value({key, value})) {
            return false;
        }
    }
    return true;
}

JsonAdapter::JsonAdapter(
    ModelFactory* model_factory, Serializeable::Format format) :
    StringAdapter(model_factory, format)
{
}

void JsonAdapter::dict_to_string(
    const Dictionary& dict, std::string& output) const
{
    if (dict.is_empty()) {
        return;
    }
    JsonUtils::to_json(dict, output, m_serializer_exclude_keys);
}

void JsonAdapter::dict_from_string(
    const std::string& input, Dictionary& dict) const
{
    if (input.empty()) {
        return;
    }
    JsonUtils::from_json(input, dict, m_deserializer_exclude_keys);
}

KeyValueAdapter::KeyValueAdapter(
    ModelFactory* model_factory, Serializeable::Format format) :
    StringAdapter(model_factory, format)
{
}

void KeyValueAdapter::dict_to_string(
    const Dictionary& dict, std::string& output) const
{
    if (dict.is_empty()) {
        return;
    }

    if (dict.get_type() == Dictionary::Type::SEQUENCE) {
        std::size_t count{0};
        for (const auto& dict_item : dict.get_sequence()) {
            Map flat;
            dict_item->flatten(flat);
            output += flat.to_string();

            if (count < dict.get_sequence().size() - 1) {
                output += '\n';
            }
            count++;
        }
    }
    else if (dict.get_type() == Dictionary::Type::MAP) {
        Map flat;
        dict.flatten(flat);
        output = flat.to_string();
    }
}

void KeyValueAdapter::dict_from_string(
    const std::string& input, Dictionary& dict) const
{
    if (input.empty()) {
        return;
    }

    std::vector<std::string> lines;
    StringUtils::to_lines(input, lines);

    std::vector<std::pair<std::size_t, std::size_t>> blank_offsets;
    std::size_t count{0};
    std::size_t last_offset{0};
    for (const auto& line : lines) {
        if (line.empty()) {
            blank_offsets.push_back({count, count - last_offset});
            last_offset = count;
        }
        count++;
    }

    if (!blank_offsets.empty()) {
        dict.set_type(Dictionary::Type::SEQUENCE);
        for (const auto& [end, length] : blank_offsets) {
            Map flat;
            for (std::size_t idx = end - length; idx < end; idx++) {
                const auto& [key, value] =
                    StringUtils::split_on_first(lines[idx], ",");
                flat.set_item(key, value);
            }
            auto block_dict = std::make_unique<Dictionary>();
            block_dict->expand(flat);
            dict.add_sequence_item(std::move(block_dict));
        }
    }
    else {
        Map flat;
        for (const auto& line : lines) {
            const auto& [key, value] = StringUtils::split_on_first(line, ",");
            flat.set_item(key, value);
        }
        dict.expand(flat);
    }
}

AdapterCollection::AdapterCollection(
    std::unique_ptr<ModelFactory> model_factory) :
    m_model_factory(std::move(model_factory))
{
}

const ModelFactory* AdapterCollection::get_model_factory() const
{
    return m_model_factory.get();
}

void AdapterCollection::add_adapter(
    const std::string& label, std::unique_ptr<StringAdapter> adapter)
{
    if (m_default_adapter.empty()) {
        m_default_adapter = label;
    }
    m_adapters[label] = std::move(adapter);
}

std::string AdapterCollection::get_type() const
{
    return m_model_factory->get_type();
}

StringAdapter* AdapterCollection::get_default_adapter() const
{
    return get_adapter(m_default_adapter);
}

StringAdapter* AdapterCollection::get_adapter(const std::string& label) const
{
    if (auto iter = m_adapters.find(label); iter != m_adapters.end()) {
        return iter->second.get();
    }
    return nullptr;
}
}  // namespace hestia