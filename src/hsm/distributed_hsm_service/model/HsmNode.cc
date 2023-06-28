#include "HsmNode.h"

namespace hestia {
HsmNode::HsmNode() : Model("hsm_node") {}

HsmNode::HsmNode(const Uuid& id) : Model(id) {}

void HsmNode::deserialize(const Dictionary& dict, SerializeFormat format)
{
    Model::deserialize(dict, format);

    if (format == SerializeFormat::ID) {
        return;
    }

    Metadata scalar_data;
    dict.get_map_items(scalar_data, {"backends"});

    auto each_item = [this](const std::string& key, const std::string& value) {
        if (key == "app_type") {
            m_app_type = value;
        }
        else if (key == "host_address") {
            m_host_address = value;
        }
        else if (key == "port") {
            m_port = value;
        }
        else if (key == "version") {
            m_version = value;
        }
        else if (key == "controller") {
            m_is_controller = value == "1";
        }
    };
    scalar_data.for_each_item(each_item);

    if (dict.has_map_item("backends")) {
        auto backends_dict = dict.get_map_item("backends");
        for (const auto& backend_dict : backends_dict->get_sequence()) {

            HsmObjectStoreClientBackend backend;
            backend.deserialize(*backend_dict);
            m_backends.push_back(backend);
        }
    }
}

void HsmNode::serialize(
    Dictionary& dict, SerializeFormat format, const Uuid& id) const
{
    Model::serialize(dict, format, id);

    if (format == SerializeFormat::ID) {
        return;
    }

    dict.set_map({
        {"app_type", m_app_type},
        {"host_address", m_host_address},
        {"port", m_port},
        {"version", m_version},
        {"controller", std::to_string(static_cast<int>(m_is_controller))},
    });

    auto backend_seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& backend : m_backends) {
        backend_seq->add_sequence_item(backend.serialize());
    }
    dict.set_map_item("backends", std::move(backend_seq));
}
}  // namespace hestia