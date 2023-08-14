#include "S3ObjectAdapter.h"

namespace hestia {
S3ObjectAdapter::S3ObjectAdapter(const std::string& metadata_prefix) :
    StringAdapter(nullptr), m_metadata_prefix(metadata_prefix)
{
}

S3ObjectAdapter::Ptr S3ObjectAdapter::create(const std::string& metadata_prefix)
{
    return std::make_unique<S3ObjectAdapter>(metadata_prefix);
}

void S3ObjectAdapter::dict_from_string(
    const std::string& input, Dictionary& dict) const
{
    (void)input;
    (void)dict;
}

void S3ObjectAdapter::dict_to_string(
    const Dictionary& dict, std::string& output) const
{
    (void)dict;
    (void)output;
}

void S3ObjectAdapter::get_headers(
    const Dataset& dataset, const HsmObject& object, Map& header) const
{
    header.set_item(
        "Creation-Time", std::to_string(object.get_creation_time()));
    header.set_item("Bucket", dataset.name());
}

}  // namespace hestia