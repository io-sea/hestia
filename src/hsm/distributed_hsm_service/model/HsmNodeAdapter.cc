#include "HsmNodeAdapter.h"

#include "Dictionary.h"
#include "JsonUtils.h"

namespace hestia {

void HsmNodeJsonAdapter::to_string(
    const std::vector<HsmNode>& items, std::string& output) const
{
    auto seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& item : items) {
        auto dict = std::make_unique<Dictionary>();
        to_dict(item, *dict);
        seq->add_sequence_item(std::move(dict));
    }
    output = JsonUtils::to_json(*seq);
}

bool HsmNodeJsonAdapter::matches_query(
    const HsmNode& item, const Metadata& query) const
{
    for (const auto& [key, value] : query.get_raw_data()) {
        if (key == "tag" && item.m_tag != value) {
            return false;
        }
        else if (key == "host_address" && item.m_host_address != value) {
            return false;
        }
    }
    return true;
}

void HsmNodeJsonAdapter::to_dict(
    const HsmNode& node, Dictionary& dict, const std::string& id)
{
    std::string working_id;
    if (!id.empty()) {
        working_id = id;
    }
    else {
        working_id = node.id();
    }

    dict.set_map({
        {"id", working_id},
        {"app_type", node.m_app_type},
        {"host_address", node.m_host_address},
        {"port", node.m_port},
        {"tag", node.m_tag},
        {"version", node.m_version},
        {"controller", std::to_string(static_cast<int>(node.m_is_controller))},
    });

    auto backend_seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& backend : node.m_backends) {
        auto backend_data = std::make_unique<Dictionary>();
        backend_data->set_map({{"identifier", backend.m_identifier}});
        backend_seq->add_sequence_item(std::move(backend_data));
    }
    dict.set_map_item("backends", std::move(backend_seq));
}

void HsmNodeJsonAdapter::to_string(
    const HsmNode& node, std::string& json, const std::string& id) const
{
    Dictionary data;
    to_dict(node, data, id);
    json = JsonUtils::to_json(data);
}

void HsmNodeJsonAdapter::from_string(
    const std::string& json, HsmNode& node) const
{
    auto dict = JsonUtils::from_json(json);
    from_dict(*dict, node);
}

void HsmNodeJsonAdapter::from_dict(const Dictionary& dict, HsmNode& node)
{
    Metadata scalar_data;
    dict.get_map_items(scalar_data, {"backends"});

    auto each_item = [&node](const std::string& key, const std::string& value) {
        if (key == "id") {
            node.m_id = value;
        }
        else if (key == "app_type") {
            node.m_app_type = value;
        }
        else if (key == "host_address") {
            node.m_host_address = value;
        }
        else if (key == "tag") {
            node.m_tag = value;
        }
        else if (key == "port") {
            node.m_port = value;
        }
        else if (key == "version") {
            node.m_version = value;
        }
        else if (key == "controller") {
            node.m_is_controller = value == "1";
        }
    };
    scalar_data.for_each_item(each_item);

    if (dict.has_map_item("backends")) {
        auto backends_dict = dict.get_map_item("backends");
        for (const auto& backend_dict : backends_dict->get_sequence()) {
            Metadata backend_data;
            backend_dict->get_map_items(backend_data);

            HsmObjectStoreClientBackend backend;
            auto on_each_backend_item =
                [&backend](const std::string& key, const std::string& value) {
                    if (key == "identifier") {
                        backend.m_identifier = value;
                    }
                };
            backend_data.for_each_item(on_each_backend_item);
            node.m_backends.push_back(backend);
        }
    }
}

void HsmNodeJsonAdapter::from_string(
    const std::string& json, std::vector<HsmNode>& items) const
{
    auto dict = JsonUtils::from_json(json);
    if (dict->get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& entry : dict->get_sequence()) {
            items.emplace_back(HsmNode());
            from_dict(*entry, items[items.size() - 1]);
        }
    }
}

}  // namespace hestia
