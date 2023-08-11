#include "Dataset.h"

namespace hestia {
Dataset::Dataset() :
    HsmItem(HsmItem::Type::DATASET),
    LockableOwnableModel(HsmItem::dataset_name, {})
{
    init();
}

Dataset::Dataset(const std::string& id) :
    HsmItem(HsmItem::Type::DATASET),
    LockableOwnableModel(id, HsmItem::dataset_name, {})
{
    init();
}

Dataset::Dataset(const Dataset& other) :
    HsmItem(HsmItem::Type::DATASET), LockableOwnableModel(other)
{
    *this = other;
}

Dataset& Dataset::operator=(const Dataset& other)
{
    if (this != &other) {
        LockableOwnableModel::operator=(other);
        m_objects    = other.m_objects;
        m_namespaces = other.m_namespaces;
        init();
    }
    return *this;
}

void Dataset::init()
{
    register_foreign_key_proxy_field(&m_objects);
    register_foreign_key_proxy_field(&m_namespaces);
}

std::string Dataset::get_type()
{
    return HsmItem::dataset_name;
}

const std::vector<HsmObject>& Dataset::objects() const
{
    return m_objects.models();
}

const std::vector<Namespace>& Dataset::namespaces() const
{
    return m_namespaces.models();
}

}  // namespace hestia