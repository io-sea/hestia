#include "Dataset.h"

namespace hestia {
Dataset::Dataset() : OwnableModel("dataset") {}

Dataset::Dataset(const std::string& name) : OwnableModel(name, "dataset") {}

Dataset::Dataset(const Uuid& id) : OwnableModel(id) {}

void Dataset::deserialize(const Dictionary& dict, SerializeFormat format)
{
    OwnableModel::deserialize(dict, format);

    if (format == SerializeFormat::ID) {
        return;
    }

    if (auto obj_dict = dict.get_map_item("objects"); obj_dict != nullptr) {
        auto object_format = (format == SerializeFormat::FULL) ?
                                 SerializeFormat::FULL :
                                 SerializeFormat::ID;

        if (obj_dict->get_type() == Dictionary::Type::SEQUENCE) {
            for (const auto& obj_dict_item : obj_dict->get_sequence()) {
                HsmObject object;
                object.deserialize(*obj_dict_item, object_format);
                m_objects.push_back(object);
            }
        }
    }
}

void Dataset::serialize(
    Dictionary& dict, SerializeFormat format, const Uuid& id) const
{
    OwnableModel::serialize(dict, format, id);

    if (format == SerializeFormat::ID) {
        return;
    }

    auto objects_dict =
        std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);

    auto object_format = (format == SerializeFormat::FULL) ?
                             SerializeFormat::FULL :
                             SerializeFormat::ID;

    for (const auto& object : m_objects) {
        auto obj_dict = std::make_unique<Dictionary>();
        object.serialize(*obj_dict, object_format);
        objects_dict->add_sequence_item(std::move(obj_dict));
    }
    dict.set_map_item("objects", std::move(objects_dict));
}
}  // namespace hestia