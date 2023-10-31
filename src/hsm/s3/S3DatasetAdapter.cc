#include "S3DatasetAdapter.h"

#include "Dataset.h"
#include "TimeUtils.h"
#include "XmlUtils.h"
#include <sstream>

namespace hestia {
S3DatasetAdapter::S3DatasetAdapter(const std::string& metadata_prefix) :
    m_metadata_prefix(metadata_prefix)
{
}

S3DatasetAdapter::Ptr S3DatasetAdapter::create(
    const std::string& metadata_prefix)
{
    return std::make_unique<S3DatasetAdapter>(metadata_prefix);
}

void S3DatasetAdapter::on_list_buckets(
    const VecModelPtr& items, S3ListBucketResponse& list_bucket_response)
{
    for (const auto& dataset : items) {
        const auto timestamp =
            TimeUtils::to_iso8601_basic(dataset->get_creation_time());
        list_bucket_response.m_buckets.push_back(
            {dataset->name(), S3Timestamp(timestamp)});
    }
}

}  // namespace hestia