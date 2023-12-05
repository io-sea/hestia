#include "S3Responses.h"

#include "ErrorUtils.h"
#include "XmlDocument.h"
#include "XmlElement.h"
#include "XmlParser.h"

#include <iostream>
#include <stdexcept>

namespace hestia {

S3Response::S3Response(std::unique_ptr<HttpResponse> http_response)
{
    if (http_response->error()) {
        m_status = S3Status(*http_response);
    }
    m_http_response = std::move(http_response);
}

bool S3Response::is_ok() const
{
    return m_status.is_ok();
}

S3ListBucketResponse::S3ListBucketResponse(const S3Response& response)
{
    if (response.is_ok()) {
        deserialize(response.m_http_response->body());
    }
    else {
        m_error = response.m_status;
    }
}

bool S3ListBucketResponse::ok() const
{
    return m_error.is_ok();
}

std::size_t S3ListBucketResponse::get_num_buckets() const
{
    return m_buckets.size();
}

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
            SOURCE_LOC()
            + " | No root element in parsed xml response. Body is: "
            + response_body);
    }

    if (xml_doc->get_root()->get_tag_name() != "ListAllMyBucketsResult") {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Missing required tag: ListAllMyBucketsResult pasring xml response. Body is: "
            + response_body);
        return;
    }

    for (const auto& child : xml_doc->get_root()->get_children()) {
        if (child->get_tag_name() == "Buckets") {
            for (const auto& bucket_element : child->get_children()) {
                m_buckets.push_back(S3Bucket(*bucket_element));
            }
        }
        else if (child->get_tag_name() == "Owner") {
            m_owner = S3Owner(*child);
        }
    }
}

std::string S3ListBucketResponse::to_string() const
{
    auto root = XmlElement::create("ListAllMyBucketsResult");

    auto buckets_element = XmlElement::create("Buckets");
    for (const auto& bucket : m_buckets) {
        buckets_element->add_child(bucket.to_xml());
    }
    root->add_child(std::move(buckets_element));

    root->add_child(m_owner.to_xml());

    return XmlDocument::to_string(*root);
}

S3ListObjectsResponse::S3ListObjectsResponse(
    const S3Response& response, bool is_version_2) :
    m_is_version2(is_version_2)
{
    if (response.m_status.is_ok()) {
        deserialize(response.m_http_response->body());
    }
    else {
        m_error = response.m_status;
    }
}

std::string S3ListObjectsResponse::to_string() const
{
    auto root = XmlElement::create("ListBucketResult");

    auto trunc_element = XmlElement::create("IsTruncated");
    trunc_element->set_text(m_is_truncated ? "TRUE" : "FALSE");
    root->add_child(std::move(trunc_element));

    for (const auto& object : m_contents) {
        auto object_element = XmlElement::create("Contents");
        object.to_xml(*object_element);
        root->add_child(std::move(object_element));
    }

    if (!m_name.empty()) {
        auto e = XmlElement::create("Name");
        e->set_text(m_name);
        root->add_child(std::move(e));
    }

    if (!m_prefix.empty()) {
        auto e = XmlElement::create("Prefix");
        e->set_text(m_prefix);
        root->add_child(std::move(e));
    }

    if (!m_delimiter.empty()) {
        auto e = XmlElement::create("Delimiter");
        e->set_text(m_delimiter);
        root->add_child(std::move(e));
    }

    if (m_max_keys > 0) {
        auto e = XmlElement::create("MaxKeys");
        e->set_text(std::to_string(m_max_keys));
        root->add_child(std::move(e));
    }

    if (!m_common_prefixes.empty()) {
        auto e = XmlElement::create("CommonPrefixes");
        for (const auto& prefix : m_common_prefixes) {
            auto prefix_element = XmlElement::create("Prefix");
            prefix_element->set_text(prefix);
            e->add_child(std::move(prefix_element));
        }
        root->add_child(std::move(e));
    }

    if (!m_encoding_type.empty()) {
        auto e = XmlElement::create("EncodingType");
        e->set_text(m_encoding_type);
        root->add_child(std::move(e));
    }

    if (m_is_version2) {
        auto key_count = XmlElement::create("KeyCount");
        key_count->set_text(std::to_string(m_contents.size()));
        root->add_child(std::move(key_count));

        if (!m_continuation_token.empty()) {
            auto e = XmlElement::create("ContinuationToken");
            e->set_text(m_continuation_token);
            root->add_child(std::move(e));
        }

        if (!m_next_continuation_token.empty()) {
            auto e = XmlElement::create("NextContinuationToken");
            e->set_text(m_next_continuation_token);
            root->add_child(std::move(e));
        }

        if (!m_start_after.empty()) {
            auto e = XmlElement::create("StartAfter");
            e->set_text(m_start_after);
            root->add_child(std::move(e));
        }
    }

    return XmlDocument::to_string(*root);
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
            + " | Missing required tag: ListBucketResult pasring xml response. Body is: "
            + response_body);
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
            m_contents.push_back(S3Object(*child));
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