#include "HsmNodeAdapter.h"

#include "Dictionary.h"
#include "JsonUtils.h"

namespace hestia {

std::string HsmNodeJsonAdapter::to_json(const std::vector<HsmNode>& nodes)
{
    std::vector<Metadata> node_data;
    for (const auto& node : nodes) {
        Metadata data;
        data.set_item("id", node.id());
        data.set_item("app_type", node.m_app_type);
        data.set_item("host_address", node.m_host_address);
        node_data.push_back(data);
    }
    return JsonUtils::to_json(node_data);
}

void HsmNodeJsonAdapter::to_string(const HsmNode& node, std::string& json) const
{
    Dictionary data;
    data.set_map({
        {"id", node.id()},
        {"app_type", node.m_app_type},
        {"host_address", node.m_host_address},
    });

    auto backend_seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& backend : node.m_backends) {
        auto backend_data = std::make_unique<Dictionary>();
        backend_data->set_map({{"identifier", backend.m_identifier}});
        backend_seq->add_sequence_item(std::move(backend_data));
    }
    data.set_map_item("backends", std::move(backend_seq));
    json = JsonUtils::to_json(data);
}

void HsmNodeJsonAdapter::from_string(
    const std::string& json, HsmNode& node) const
{
    auto dict = JsonUtils::from_json(json);

    Metadata scalar_data;
    dict->get_map_items(scalar_data, {"backends"});

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
    };
    scalar_data.for_each_item(each_item);

    if (dict->has_map_item("backends")) {
        auto backends_dict = dict->get_map_item("backends");
        for (const auto& backend_dict : backends_dict->get_sequence()) {
            Metadata backend_data;
            backend_dict->get_map_items(backend_data);

            ObjectStoreBackend backend;
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

}  // namespace hestia
