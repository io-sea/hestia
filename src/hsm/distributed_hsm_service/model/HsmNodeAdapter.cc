#include "HsmNodeAdapter.h"

#include "JsonUtils.h"
#include "Metadata.h"

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
    Metadata data;
    data.set_item("id", node.id());
    data.set_item("app_type", node.m_app_type);
    data.set_item("host_address", node.m_host_address);
    json = JsonUtils::to_json(data);
}

void HsmNodeJsonAdapter::from_string(
    const std::string& json, HsmNode& node) const
{
    Metadata data;
    JsonUtils::from_json(json, data);

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
    data.for_each_item(each_item);
}

}  // namespace hestia
