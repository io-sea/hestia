#include "DatasetAdapter.h"

#include "JsonUtils.h"
#include "Metadata.h"

namespace hestia {

void DatasetJsonAdapter::to_string(
    const std::vector<Dataset>& items, std::string& output) const
{
    auto seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& item : items) {
        auto dict = std::make_unique<Dictionary>();
        to_dict(item, *dict);
        seq->add_sequence_item(std::move(dict));
    }
    output = JsonUtils::to_json(*seq);
}

void DatasetJsonAdapter::to_dict(
    const Dataset& item, Dictionary& dict, const std::string& id) const
{
    std::string working_id;
    if (!id.empty()) {
        working_id = id;
    }
    else {
        working_id = item.id();
    }
    std::unordered_map<std::string, std::string> data{{"id", working_id}};
    dict.set_map(data);
}

void DatasetJsonAdapter::from_dict(const Dictionary& dict, Dataset& item) const
{
    Metadata scalar_data;
    dict.get_map_items(scalar_data);

    auto on_item = [&item](const std::string& key, const std::string& value) {
        if (key == "id" && !value.empty()) {
            item.m_identifier = value;
        }
    };
    scalar_data.for_each_item(on_item);
}

void DatasetJsonAdapter::to_string(
    const Dataset& item, std::string& output, const std::string& id) const
{
    Dictionary dict;
    to_dict(item, dict, id);
    output = JsonUtils::to_json(dict);
}

void DatasetJsonAdapter::from_string(
    const std::string& input, Dataset& item) const
{
    auto dict = JsonUtils::from_json(input);
    from_dict(*dict, item);
}

void DatasetJsonAdapter::from_string(
    const std::string& json, std::vector<Dataset>& items) const
{
    auto dict = JsonUtils::from_json(json);
    if (dict->get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& entry : dict->get_sequence()) {
            items.emplace_back(Dataset());
            from_dict(*entry, items[items.size() - 1]);
        }
    }
}

}  // namespace hestia