#include "YamlUtils.h"

#include "Dictionary.h"

#include "yaml-cpp/yaml.h"

#include <cstddef>
#include <iostream>
#include <memory>
#include <string>

namespace hestia {

// Helper functions
void load_node_to_dict(const YAML::Node& root, Dictionary& dict);
void on_map(const YAML::Node& node, Dictionary& dict);
void on_sequence(const YAML::Node& node, Dictionary& dict);

void on_map_item(
    const std::string& key,
    const Dictionary* dict,
    YAML::Emitter& emitter,
    const bool sorted = false);
void on_tag(const Dictionary& dict, YAML::Emitter& emitter);
void on_map(
    const Dictionary& dict, YAML::Emitter& emitter, const bool sorted = false);
void on_sequence(
    const Dictionary& dict, YAML::Emitter& emitter, const bool sorted = false);

void YamlUtils::load(const std::string& path, Dictionary& dict)
{
    if (dict.get_type() != Dictionary::Type::MAP) {
        return;
    }

    YAML::Node root = YAML::LoadFile(path);
    load_node_to_dict(root, dict);
}

void YamlUtils::load_all(
    const std::string& path,
    std::vector<std::unique_ptr<hestia::Dictionary>>& dicts)
{
    std::vector<YAML::Node> roots = YAML::LoadAllFromFile(path);

    for (std::size_t i = 0; i < roots.size(); i++) {
        dicts.push_back(Dictionary::create(Dictionary::Type::MAP));
        load_node_to_dict(roots[i], *(dicts[i]));
    }
}

void YamlUtils::dict_to_yaml(
    const Dictionary& dict, std::string& yaml, const bool sorted)
{
    if (dict.get_type() != Dictionary::Type::MAP) {
        return;
    }

    auto root = dict.get_map_item("root");
    if (root == nullptr) {
        return;
    }

    YAML::Emitter emitter;
    emitter << YAML::BeginDoc;
    if (root->get_type() == Dictionary::Type::MAP) {
        on_tag(*root, emitter);
        on_map(*root, emitter, sorted);
    }
    else if (root->get_type() == Dictionary::Type::SEQUENCE) {
        on_sequence(*root, emitter, sorted);
    }
    else if (root->get_type() == Dictionary::Type::SCALAR) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "root" << YAML::Value << root->get_scalar();
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndDoc;
    yaml += emitter.c_str();
}


void load_node_to_dict(const YAML::Node& root, Dictionary& dict)
{
    if (!root.IsDefined()) {
        return;
    }

    if (root.IsNull()) {
        dict.set_map_item("root", nullptr);
    }

    else if (root.IsSequence()) {
        auto seq_dict = Dictionary::create(Dictionary::Type::SEQUENCE);
        auto raw_dict = seq_dict.get();
        dict.set_map_item("root", std::move(seq_dict));
        on_sequence(root, *raw_dict);
    }
    else if (root.IsMap()) {
        auto map_dict = Dictionary::create(Dictionary::Type::MAP);
        auto raw_dict = map_dict.get();
        dict.set_map_item("root", std::move(map_dict));
        on_map(root, *raw_dict);
    }
    else if (root.IsScalar()) {
        auto scalar_dict = Dictionary::create(Dictionary::Type::SCALAR);
        scalar_dict->set_scalar(root.as<std::string>());
        dict.set_map_item("root", std::move(scalar_dict));
    }
}

void on_tag(const Dictionary& dict, YAML::Emitter& emitter)
{
    if (dict.has_tag()) {
        if (dict.get_tag().first.empty()) {
            emitter << YAML::LocalTag(dict.get_tag().second);
        }
        else if (dict.get_tag().first == "!!") {
            emitter << YAML::LocalTag("", dict.get_tag().second);
        }
        else {  // Skip first ! to harmonise
            emitter << YAML::LocalTag(
                dict.get_tag().first.substr(1), dict.get_tag().second);
        }
    }
}

void on_map_item(
    const std::string& key,
    const Dictionary* value,
    YAML::Emitter& emitter,
    const bool sorted)
{
    emitter << YAML::Key << key;

    on_tag(*value, emitter);

    emitter << YAML::Value;

    if (value->is_empty()) {
        emitter << YAML::Null;
    }
    else {
        switch (value->get_type()) {
            case Dictionary::Type::MAP:
                on_map(*value, emitter, sorted);
                break;
            case Dictionary::Type::SEQUENCE:
                on_sequence(*value, emitter, sorted);
                break;
            case Dictionary::Type::SCALAR:
                if (value->should_quote_scalar()) {
                    emitter << YAML::DoubleQuoted << value->get_scalar();
                }
                else {
                    emitter << value->get_scalar();
                }
        }
    }
}

void on_map(const Dictionary& dict, YAML::Emitter& emitter, const bool sorted)
{
    if (dict.is_empty()) {
        return;
    }
    emitter << YAML::BeginMap;

    if (sorted) {
        std::vector<std::string> keys;
        keys.reserve(dict.get_map().size());

        for (const auto& elem : dict.get_map()) {
            keys.push_back(elem.first);
        }

        std::sort(keys.begin(), keys.end());

        for (const auto& key : keys) {
            on_map_item(key, dict.get_map_item(key), emitter, sorted);
        }
    }
    else {
        for (const auto& [key, value] : dict.get_map()) {
            on_map_item(key, value.get(), emitter, sorted);
        }
    }

    emitter << YAML::EndMap;
}

void on_sequence(
    const Dictionary& dict, YAML::Emitter& emitter, const bool sorted)
{
    emitter << YAML::BeginSeq;
    for (const auto& value : dict.get_sequence()) {
        on_tag(*value, emitter);
        if (value->get_type() == Dictionary::Type::MAP) {
            on_map(*value, emitter, sorted);
        }
        if (value->get_type() == Dictionary::Type::SCALAR) {
            if (value->should_quote_scalar()) {
                emitter << YAML::DoubleQuoted << value->get_scalar();
            }
            else {
                emitter << value->get_scalar();
            }
        }
    }
    emitter << YAML::EndSeq;
}

void on_map(const YAML::Node& node, Dictionary& dict)
{
    for (const auto& kv_pair : node) {
        if (kv_pair.first.IsScalar()) {
            const auto key = kv_pair.first.as<std::string>();
            if (!kv_pair.second.IsDefined() || kv_pair.second.IsNull()) {
                continue;
            }

            if (kv_pair.second.IsSequence()) {
                auto seq_dict = Dictionary::create(Dictionary::Type::SEQUENCE);
                auto raw_dict = seq_dict.get();
                dict.set_map_item(key, std::move(seq_dict));
                on_sequence(kv_pair.second, *raw_dict);
            }
            else if (kv_pair.second.IsMap()) {
                auto map_dict = Dictionary::create(Dictionary::Type::MAP);
                auto raw_dict = map_dict.get();
                dict.set_map_item(key, std::move(map_dict));
                on_map(kv_pair.second, *raw_dict);
            }
            else if (kv_pair.second.IsScalar()) {
                auto scalar_dict = Dictionary::create(Dictionary::Type::SCALAR);
                scalar_dict->set_scalar(kv_pair.second.as<std::string>());
                dict.set_map_item(key, std::move(scalar_dict));
            }
        }
    }
}

void on_sequence(const YAML::Node& node, Dictionary& dict)
{
    for (const auto& seq_node : node) {
        if (!seq_node.IsDefined() || seq_node.IsNull()) {
            continue;
        }

        if (seq_node.IsSequence()) {
            auto seq_dict = Dictionary::create(Dictionary::Type::SEQUENCE);
            auto raw_dict = seq_dict.get();
            dict.add_sequence_item(std::move(seq_dict));
            on_sequence(seq_node, *raw_dict);
        }
        else if (seq_node.IsMap()) {
            auto map_dict = Dictionary::create(Dictionary::Type::MAP);
            auto raw_dict = map_dict.get();
            dict.add_sequence_item(std::move(map_dict));
            on_map(seq_node, *raw_dict);
        }
        else if (seq_node.IsScalar()) {
            auto scalar_dict = Dictionary::create(Dictionary::Type::SCALAR);
            scalar_dict->set_scalar(seq_node.as<std::string>());
            dict.add_sequence_item(std::move(scalar_dict));
        }
    }
}

}  // namespace hestia
