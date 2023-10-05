#include "S3Client.h"

#include "HttpRequest.h"
#include "XmlAttribute.h"
#include "XmlDocument.h"
#include "XmlElement.h"

namespace hestia {
S3Client::S3Client(HttpClient* http_client) : m_http_client(http_client) {}

S3Client::~S3Client() {}

S3Status S3Client::create_bucket(
    const S3Bucket& bucket, const S3Request& request) const
{
    const auto status = bucket.validate_name(request.m_tracking_id);
    if (!status.is_ok()) {
        return status;
    }

    std::string path = request.m_endpoint;
    if (request.m_uri_style == S3UriStyle::PATH) {
        path += "/" + bucket.name();
    }

    HttpRequest http_request(path, HttpRequest::Method::PUT);
    prepare_headers(http_request.get_header(), path);

    http_request.get_header().set_item(
        "x-amz-content-sha256", "UNSIGNED-PAYLOAD");

    XmlDocument xml_doc;
    auto create_element = XmlElement::create("CreateBucketConfiguration");
    auto xml_ns_attr    = XmlAttribute::create("xmlns");
    xml_ns_attr->set_value("http://s3.amazonaws.com/doc/2006-03-01/");
    create_element->add_attribute(std::move(xml_ns_attr));
    if (!bucket.get_location_constraint().empty()) {
        create_element->add_child(bucket.get_location_constraint_xml());
    }

    xml_doc.set_root(std::move(create_element));
    http_request.body() = xml_doc.to_string();

    http_request.get_header().set_item(
        "authorization",
        get_authorization_header(request.m_user_context, http_request));
    return do_request(http_request)->m_status;
}

S3Status S3Client::head_bucket(
    const S3Bucket& bucket, const S3Request& request) const
{
    std::string path = request.m_endpoint;
    if (request.m_uri_style == S3UriStyle::PATH) {
        path += "/" + bucket.name();
    }

    HttpRequest http_request(path, HttpRequest::Method::HEAD);
    prepare_headers(http_request.get_header(), path);
    http_request.get_header().set_item(
        "authorization",
        get_authorization_header(request.m_user_context, http_request));
    return do_request(http_request)->m_status;
}

S3Status S3Client::put_object(
    const S3Object& object,
    const S3Bucket& bucket,
    const S3Request& request,
    Stream* stream)
{
    std::string path = request.m_endpoint;
    if (request.m_uri_style == S3UriStyle::PATH) {
        path += "/" + bucket.name() + "/" + object.m_key;
    }
    else {
        path = "/" + object.m_key;
    }

    HttpRequest http_request(path, HttpRequest::Method::PUT);
    prepare_headers(http_request.get_header(), path);

    http_request.get_header().set_item(
        "x-amz-content-sha256", "UNSIGNED-PAYLOAD");

    auto response = std::make_unique<S3Response>(
        m_http_client->make_request(http_request, stream));
    return response->m_status;
}

S3Status S3Client::delete_object(
    const S3Object& object,
    const S3Bucket& bucket,
    const S3Request& request) const
{
    std::string path = request.m_endpoint;
    if (request.m_uri_style == S3UriStyle::PATH) {
        path += "/" + bucket.name() + "/" + object.m_key;
    }
    else {
        path = "/" + object.m_key;
    }

    HttpRequest http_request(path, HttpRequest::Method::DELETE);
    prepare_headers(http_request.get_header(), path);

    return do_request(http_request)->m_status;
}

S3Status S3Client::get_object(
    const S3Object& object,
    const S3Bucket& bucket,
    const S3Request& request,
    Stream* stream)
{
    std::string path = request.m_endpoint;
    if (request.m_uri_style == S3UriStyle::PATH) {
        path += "/" + bucket.name() + "/" + object.m_key;
    }
    else {
        path = "/" + object.m_key;
    }

    HttpRequest http_request(path, HttpRequest::Method::GET);
    prepare_headers(http_request.get_header(), path);

    auto response = std::make_unique<S3Response>(
        m_http_client->make_request(http_request, stream));
    return response->m_status;
}

S3ListObjectsResponse::Ptr S3Client::list_objects(
    const S3Bucket& bucket, const S3ListObjectRequest& request)
{
    std::string path = request.m_s3_request.m_endpoint;
    if (request.m_s3_request.m_uri_style == S3UriStyle::PATH) {
        path += "/" + bucket.name();
    }

    HttpRequest http_request(path, HttpRequest::Method::GET);
    prepare_headers(http_request.get_header(), path);

    Map query;
    request.build_query(query);
    http_request.set_queries(query);

    http_request.get_header().set_item(
        "authorization",
        get_authorization_header(
            request.m_s3_request.m_user_context, http_request));

    const auto s3_response = do_request(http_request);

    auto list_response =
        std::make_unique<S3ListObjectsResponse>(request.m_is_v2_type);
    if (s3_response->m_status.is_ok()) {
        list_response->deserialize(s3_response->m_http_response->body());
    }
    else {
        list_response->m_error = s3_response->m_status;
    }
    return list_response;
}

S3Status S3Client::delete_bucket(
    const S3Bucket& bucket, const S3Request& request) const
{
    std::string path = request.m_endpoint;
    if (request.m_uri_style == S3UriStyle::PATH) {
        path += "/" + bucket.name();
    }

    HttpRequest http_request(path, HttpRequest::Method::DELETE);
    prepare_headers(http_request.get_header(), path);
    http_request.get_header().set_item(
        "authorization",
        get_authorization_header(request.m_user_context, http_request));
    return do_request(http_request)->m_status;
}

S3ListBucketResponse::Ptr S3Client::list_buckets(const S3Request& request) const
{
    HttpRequest http_request(request.m_endpoint, HttpRequest::Method::GET);
    prepare_headers(http_request.get_header(), request.m_endpoint);
    http_request.get_header().set_item(
        "authorization",
        get_authorization_header(request.m_user_context, http_request));

    const auto s3_response = do_request(http_request);

    auto list_response = std::make_unique<S3ListBucketResponse>();
    if (s3_response->m_status.is_ok()) {
        list_response->deserialize(s3_response->m_http_response->body());
    }
    else {
        list_response->m_error = s3_response->m_status;
    }
    return list_response;
}

void S3Client::prepare_headers(
    HttpHeader& header, const std::string& path) const
{
    header.set_item("content-type", "application/xml");
    header.set_item("host", path);
}

std::string S3Client::get_authorization_header(
    const S3UserContext& user_context, const HttpRequest& request) const
{
    (void)user_context;
    (void)request;
    return {};
}

S3Response::Ptr S3Client::do_request(const HttpRequest& http_request) const
{
    return std::make_unique<S3Response>(
        m_http_client->make_request(http_request));
}

}  // namespace hestia