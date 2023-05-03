#include "S3ContainerAdapter.h"

#include "S3ContainerModel.h"

namespace hestia {
S3ContainerAdapter::S3ContainerAdapter(const std::string& metadata_prefix) :
    m_metadata_prefix(metadata_prefix)
{
}

void S3ContainerAdapter::to_s3(
    S3Container& container, const StorageObject& storage_object)
{
    S3ContainerModel::deserialize(
        container, storage_object.m_metadata, m_metadata_prefix);
}

Metadata::Query S3ContainerAdapter::get_query_filter() const
{
    return S3ContainerModel::get_query_filter(m_metadata_prefix);
}

void S3ContainerAdapter::from_s3(
    StorageObject& storage_object, const S3Container& container)
{
    storage_object.m_id = container.m_name;
    storage_object.m_metadata =
        S3ContainerModel::serialize(container, m_metadata_prefix);
}
}  // namespace hestia