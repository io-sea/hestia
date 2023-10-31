#include "ModelSerializer.h"

#include "ErrorUtils.h"
#include "JsonUtils.h"
#include "StringUtils.h"

#include <stdexcept>

namespace hestia {
ModelSerializer::ModelSerializer(
    ModelFactory::Ptr model_factory, Serializeable::Format format) :
    m_model_factory(std::move(model_factory)), m_format(format)
{
}

ModelSerializer::~ModelSerializer() {}

Model* ModelSerializer::get_template() const
{
    return m_template.get();
}

void ModelSerializer::load_template()
{
    m_template = m_model_factory->create();
}

void ModelSerializer::to_dict(
    const Model& item,
    Dictionary& output,
    const Dictionary& override,
    Serializeable::Format format_in) const
{
    const auto format =
        format_in == Serializeable::Format::UNSET ? m_format : format_in;
    item.serialize(output, format);
    output.merge(override);
}

void ModelSerializer::to_dict(
    const VecModelPtr& items,
    Dictionary& output,
    const std::vector<Dictionary>& overrides,
    const Index& index,
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
    else if (index.is_set()) {
        if (index.value() >= items.size()) {
            throw std::range_error(
                SOURCE_LOC()
                + " | Out of bounds access attempted in string adapter conversion");
        }
        items[index.value()]->serialize(output, format);
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

void ModelSerializer::from_dict(
    const Dictionary& dict, VecModelPtr& items) const
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

bool ModelSerializer::matches_query(const Model& item, const Map& query) const
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

}  // namespace hestia