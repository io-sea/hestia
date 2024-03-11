#include "S3HsmObjectAdapter.h"

#include "TimeUtils.h"

namespace hestia {
S3HsmObjectAdapter::S3HsmObjectAdapter(const std::string& metadata_prefix) :
    m_metadata_prefix(metadata_prefix)
{
}

S3HsmObjectAdapter::Ptr S3HsmObjectAdapter::create(
    const std::string& metadata_prefix)
{
    return std::make_unique<S3HsmObjectAdapter>(metadata_prefix);
}

void S3HsmObjectAdapter::get_headers(
    const std::string& bucket_name, const HsmObject& object, Map& header) const
{
    header.set_item(
        "Creation-Time",
        TimeUtils::micros_to_string(object.get_creation_time()));
    header.set_item("Bucket", bucket_name);
    for (const auto& [key, value] : object.metadata().data()) {
        header.set_item(key, value);
    }
}

void S3HsmObjectAdapter::on_list_objects(
    const Dataset& dataset, S3ListObjectsResponse& list_objects_response)
{
    for (const auto& object : dataset.objects()) {
        S3Object s3_object;
        s3_object.m_bucket = dataset.name();
        s3_object.m_key    = object.name();
        s3_object.m_last_modified =
            TimeUtils::to_iso8601_basic(object.get_last_modified_time());
        s3_object.m_size = object.size();
        list_objects_response.m_contents.push_back(s3_object);
    }
    list_objects_response.m_key_count = dataset.objects().size();
}

}  // namespace hestia