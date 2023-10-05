#include "S3Responses.h"

#include "ErrorUtils.h"
#include "XmlDocument.h"
#include "XmlElement.h"
#include "XmlParser.h"

#include <stdexcept>

namespace hestia {
void S3ListBucketResponse::deserialize(const std::string& response_body)
{
    if (response_body.empty()) {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Empty request body handling list bucket response.");
    }

    XmlParser parser;
    const auto xml_doc = parser.run(response_body);
    if (!xml_doc->has_root()) {
        throw std::runtime_error(
            SOURCE_LOC() + " | No root element in parsed xml response.");
    }

    if (xml_doc->get_root()->get_tag_name() != "ListAllMyBucketsResult") {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Missing required tag: ListAllMyBucketsResult pasring xml response.");
        return;
    }

    for (const auto& child : xml_doc->get_root()->get_children()) {
        if (child->get_tag_name() == "Buckets") {
            for (const auto& bucket_element : child->get_children()) {
                m_buckets.push_back(
                    std::make_unique<S3Bucket>(*bucket_element));
            }
        }
        else if (child->get_tag_name() == "Owner") {
            m_owner = S3Owner(*child);
        }
    }
}

void S3ListObjectsResponse::deserialize(const std::string& response_body)
{
    if (response_body.empty()) {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Empty request body handling list object response.");
    }

    XmlParser parser;
    const auto xml_doc = parser.run(response_body);
    if (!xml_doc->has_root()) {
        throw std::runtime_error(
            SOURCE_LOC() + " | No root element in parsed xml response.");
    }

    if (xml_doc->get_root()->get_tag_name() != "ListBucketResult") {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Missing required tag: ListBucketResult pasring xml response.");
        return;
    }

    for (const auto& child : xml_doc->get_root()->get_children()) {
        if (child->get_tag_name() == "IsTruncated") {
            m_is_truncated = child->get_text() == "TRUE";
        }
        else if (child->get_tag_name() == "Marker") {
            m_marker = child->get_text();
        }
        else if (child->get_tag_name() == "NextMarker") {
            m_next_marker = child->get_text();
        }
        else if (child->get_tag_name() == "Contents") {
            for (const auto& contents_child : child->get_children()) {
                m_contents.push_back(S3Object(*contents_child));
            }
        }
        else if (child->get_tag_name() == "Name") {
            m_name = child->get_text();
        }
        else if (child->get_tag_name() == "Prefix") {
            m_prefix = child->get_text();
        }
        else if (child->get_tag_name() == "Delimiter") {
            m_delimiter = child->get_text();
        }
        else if (child->get_tag_name() == "MaxKeys") {
            m_max_keys = std::stoull(child->get_text());
        }
        else if (child->get_tag_name() == "CommonPrefixes") {
            for (const auto& prefix_content : child->get_children()) {
                if (prefix_content->get_tag_name() == "Prefix") {
                    m_common_prefixes.push_back(prefix_content->get_text());
                }
            }
        }
        else if (child->get_tag_name() == "EncodingType") {
            m_encoding_type = std::stoull(child->get_text());
        }

        if (m_is_version2) {
            if (child->get_tag_name() == "KeyCount") {
                m_key_count = std::stoull(child->get_text());
            }
            else if (child->get_tag_name() == "ContinuationToken") {
                m_continuation_token = child->get_text();
            }
            else if (child->get_tag_name() == "NextContinuationToken") {
                m_next_continuation_token = child->get_text();
            }
            else if (child->get_tag_name() == "StartsAfter") {
                m_start_after = child->get_text();
            }
        }
    }
}
}  // namespace hestia