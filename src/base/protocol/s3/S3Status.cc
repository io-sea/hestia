#include "S3Status.h"

#include "XmlDocument.h"
#include "XmlParser.h"

#include <sstream>
#include <unordered_map>

namespace hestia {

static std::unordered_map<
    S3Status::Code,
    std::pair<HttpStatus::Code, std::pair<std::string, std::string>>>
    errors = {
        {S3Status::Code::_400_INCOMPLETE_BODY,
         {HttpStatus::Code::_400_BAD_REQUEST,
          {"IncompleteBody",
           "You did not provide the number of bytes specified by the Content-Length HTTP header."}}},
        {S3Status::Code::_400_INVALID_ARGUMENT,
         {HttpStatus::Code::_400_BAD_REQUEST,
          {"InvalidArgument", "Invalid Argument."}}},
        {S3Status::Code::_400_INVALID_BUCKET_NAME,
         {HttpStatus::Code::_400_BAD_REQUEST,
          {"InvalidBucketName", "The specified bucket is not valid."}}},
        {S3Status::Code::_400_INVALID_SIGNATURE_TYPE,
         {HttpStatus::Code::_400_BAD_REQUEST,
          {"InvalidRequest", "Please use AWS4-HMAC-SHA256."}}},
        {S3Status::Code::_400_AUTHORIZATION_HEADER_MALFORMED,
         {HttpStatus::Code::_400_BAD_REQUEST,
          {"AuthorizationHeaderMalformed",
           "The authorization header you provided is invalid."}}},
        {S3Status::Code::_403_INVALID_KEY_ID,
         {HttpStatus::Code::_403_FORBIDDEN,
          {"InvalidAccessKeyId", "The given key is not valid."}}},
        {S3Status::Code::_403_ACCESS_DENIED,
         {HttpStatus::Code::_403_FORBIDDEN, {"AccessDenied", "Access Denied"}}},
        {S3Status::Code::_403_SIGNATURE_DOES_NOT_MATCH,
         {HttpStatus::Code::_403_FORBIDDEN,
          {"SignatureDoesNotMatch",
           "The request signature we calculated does not match the signature you provided."}}},
        {S3Status::Code::_404_NO_SUCH_BUCKET,
         {HttpStatus::Code::_404_NOT_FOUND,
          {"NoSuchBucket", "The specified bucket does not exist."}}},
        {S3Status::Code::_404_NO_SUCH_KEY,
         {HttpStatus::Code::_404_NOT_FOUND,
          {"NoSuchKey", "The specified key does not exist."}}},
        {S3Status::Code::_404_NO_SUCH_VERSION,
         {HttpStatus::Code::_404_NOT_FOUND,
          {"NoSuchVersion",
           "The version ID specified in the request does not match any existing version."}}},
        {S3Status::Code::_404_NOT_IMPLEMENTED,
         {HttpStatus::Code::_404_NOT_FOUND,
          {"NotImplemented",
           "A header you provided implies functionality that is not implemented."}}},
        {S3Status::Code::_409_BUCKET_EXISTS,
         {HttpStatus::Code::_409_CONFLICT,
          {"BucketAlreadyExists",
           "The requested bucket name is not available. The bucket namespace is shared by all users of the system. Please select a different name and try again."}}},
        {S3Status::Code::_409_BUCKET_NOT_EMPTY,
         {HttpStatus::Code::_409_CONFLICT,
          {"BucketNotEmpty", "The bucket you tried to delete is not empty."}}},
        {S3Status::Code::_411_MISSING_CONTENT_LENGTH,
         {HttpStatus::Code::_411_LENGTH_REQURED,
          {"MissingContentLength",
           "You must provide the Content-Length HTTP header."}}},
        {S3Status::Code::_500_INTERNAL_SERVER_ERROR,
         {HttpStatus::Code::_400_BAD_REQUEST,
          {"InternalError",
           "We encountered an internal error. Please try again."}}},
};

S3Status::S3Status(
    Code code, const S3Request& request, const std::string& message_details) :
    HttpStatus(),
    m_s3_code(code),
    m_request_id(request.m_tracking_id),
    m_path(request.m_path),
    m_message_details(message_details)
{
    if (const auto iter = errors.find(code); iter != errors.end()) {
        m_code     = iter->second.first;
        m_code_str = iter->second.second.first;
        m_message  = iter->second.second.second;
    }
    else {
        m_code = HttpStatus::Code::_500_INTERNAL_SERVER_ERROR;
    }
}

void S3Status::from_xml(const XmlElement& element)
{
    for (const auto& child : element.get_children()) {
        if (child->get_tag_name() == "Code") {
            m_code_str = child->get_text();
        }
        else if (child->get_tag_name() == "Message") {
            m_message = child->get_text();
        }
        else if (child->get_tag_name() == "Resource") {
            m_path = S3Path(child->get_text());
        }
        else if (child->get_tag_name() == "RequestId") {
            m_request_id = child->get_text();
        }
        else {
            m_fields.push_back({child->get_tag_name(), child->get_text()});
        }
    }
}

S3Status::S3Status(const HttpResponse& http_response)
{
    if (!http_response.body().empty()) {
        XmlParser parser;
        const auto xml_doc = parser.run(http_response.body());

        if (xml_doc->has_root()) {
            auto root = xml_doc->get_root();
            if (root->get_tag_name() == "Error") {
                from_xml(*root);
            }
        }
    }

    if (m_code_str.empty()) {
        m_code = HttpStatus::get_code_from_numeric(http_response.code());
    }
    else {
        for (const auto& error : errors) {
            if (error.second.second.first == m_code_str) {
                m_s3_code = error.first;
                break;
            }
        }
    }
}

void S3Status::add_field(const std::string& key, const std::string& value)
{
    m_fields.push_back({key, value});
}

bool S3Status::is_ok() const
{
    return get_code_and_id().first < 400;
}

void S3Status::add_message_details(const std::string& details)
{
    m_message_details = details;
}

S3Status::Code S3Status::get_s3_code() const
{
    return m_s3_code;
}

XmlElementPtr S3Status::to_xml() const
{
    auto root = XmlElement::create("Error");

    auto code_element = XmlElement::create("Code");
    code_element->set_text(m_code_str);
    root->add_child(std::move(code_element));

    auto message_element = XmlElement::create("Message");
    if (m_message_details.empty()) {
        message_element->set_text(m_message);
    }
    else {
        message_element->set_text(m_message + "/n/n" + m_message_details);
    }
    root->add_child(std::move(message_element));

    auto request_id_element = XmlElement::create("RequestId");
    request_id_element->set_text(m_request_id);
    root->add_child(std::move(request_id_element));

    auto resource_element = XmlElement::create("Resource");
    resource_element->set_text(m_path.get_resource_path());
    root->add_child(std::move(resource_element));

    for (const auto& [key, value] : m_fields) {
        auto custom_element = XmlElement::create(key);
        custom_element->set_text(value);
        root->add_child(std::move(custom_element));
    }
    return root;
}

std::string S3Status::to_string() const
{
    XmlDocument xml_doc;

    xml_doc.set_root(to_xml());
    return xml_doc.to_string();
}
}  // namespace hestia