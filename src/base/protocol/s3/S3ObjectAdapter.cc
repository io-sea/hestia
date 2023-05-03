#include "S3ObjectAdapter.h"

#include "S3ObjectModel.h"

namespace hestia {
S3ObjectAdapter::S3ObjectAdapter(const std::string& metadata_prefix) :
    m_metadata_prefix(metadata_prefix)
{
}

void S3ObjectAdapter::to_s3(
    S3Object& object, const StorageObject& storage_object)
{
    S3ObjectModel::deserialize(
        object, storage_object.m_metadata, m_metadata_prefix);
}

std::string S3ObjectAdapter::get_query_filter() const
{
    return S3ObjectModel::get_query_filter(m_metadata_prefix);
}

void S3ObjectAdapter::from_s3(
    StorageObject& storage_object,
    const S3Container& container,
    const S3Object& object)
{
    storage_object.m_id = object.m_key;
    storage_object.m_metadata =
        S3ObjectModel::serialize(container, object, m_metadata_prefix);
    storage_object.m_size = std::stoi(object.m_content_length);
}
}  // namespace hestia