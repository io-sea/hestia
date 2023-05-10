#include "YamlUtils.h"

#include "yaml-cpp/yaml.h"

#include <iostream>

namespace hestia {
void YamlUtils::load(const std::string& path, Dictionary& dict)
{
    if (dict.get_type() != Dictionary::Type::MAP) {
        return;
    }

    YAML::Node root = YAML::LoadFile(path);
    if (!root.IsDefined() || root.IsNull()) {
        return;
    }

    if (root.IsSequence()) {
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

void YamlUtils::on_map(const YAML::Node& node, Dictionary& dict)
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

void YamlUtils::on_sequence(const YAML::Node& node, Dictionary& dict)
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
