#include "S3DatasetAdapter.h"

#include "Dataset.h"
#include "XmlUtils.h"
#include <sstream>

namespace hestia {
S3DatasetAdapter::S3DatasetAdapter(const std::string& metadata_prefix) :
    StringAdapter(nullptr), m_metadata_prefix(metadata_prefix)
{
}

S3DatasetAdapter::Ptr S3DatasetAdapter::create(
    const std::string& metadata_prefix)
{
    return std::make_unique<S3DatasetAdapter>(metadata_prefix);
}

void S3DatasetAdapter::dict_from_string(
    const std::string& input, Dictionary& dict, const std::string&) const
{
    (void)input;
    (void)dict;
}

void S3DatasetAdapter::dict_to_string(
    const Dictionary& dict, std::string& output) const
{
    (void)dict;
    (void)output;
}

void S3DatasetAdapter::on_list(const Dataset& dataset, std::string& output)
{
    std::stringstream sstr;
    XmlUtils::add_default_prolog(sstr);
    XmlUtils::open_tag(sstr, "ListBucketResult");

    XmlUtils::add_tag_and_text(sstr, "Name", dataset.name());
    XmlUtils::add_tag_and_text(
        sstr, "KeyCount", std::to_string(dataset.objects().size()));

    for (const auto& object : dataset.objects()) {
        XmlUtils::open_tag(sstr, "Contents");

        XmlUtils::add_tag_and_text(sstr, "Key", object.name());
        XmlUtils::add_tag_and_text(
            sstr, "LastModified",
            std::to_string(object.get_last_modified_time()));
        XmlUtils::add_tag_and_text(sstr, "Size", std::to_string(object.size()));

        XmlUtils::close_tag(sstr, "Contents");
    }
    XmlUtils::close_tag(sstr, "ListBucketResult");
    output = sstr.str();
}

void S3DatasetAdapter::on_list(const VecModelPtr& items, std::string& output)
{
    std::stringstream sstr;
    sstr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    sstr << "<ListAllMyBucketsResult>\n";
    sstr << "<Buckets>\n";

    for (const auto& dataset : items) {
        sstr << "<Bucket>\n";
        sstr << "<Name>" << dataset->name() << "</Name>\n";
        sstr << "<CreationDate>" << dataset->get_creation_time()
             << "</CreationDate>\n";
        sstr << "</Bucket>\n";
    }

    sstr << "</Buckets>\n";
    sstr << "</ListAllMyBucketsResult>\n";
    output = sstr.str();
}

}  // namespace hestia