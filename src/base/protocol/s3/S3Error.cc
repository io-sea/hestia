#include "S3Error.h"

#include "XmlDocument.h"
#include "XmlParser.h"

#include <sstream>
#include <unordered_map>

namespace hestia {

static std::unordered_map<
    S3Error::Code,
    std::pair<HttpError::Code, std::pair<std::string, std::string>>>
    errors = {
        {S3Error::Code::_400_INCOMPLETE_BODY,
         {HttpError::Code::_400_BAD_REQUEST,
          {"IncompleteBody",
           "You did not provide the number of bytes specified by the Content-Length HTTP header."}}},
        {S3Error::Code::_400_INVALID_ARGUMENT,
         {HttpError::Code::_400_BAD_REQUEST,
          {"InvalidArgument", "Invalid Argument."}}},
        {S3Error::Code::_400_INVALID_BUCKET_NAME,
         {HttpError::Code::_400_BAD_REQUEST,
          {"InvalidBucketName", "The specified bucket is not valid."}}},
        {S3Error::Code::_400_INVALID_SIGNATURE_TYPE,
         {HttpError::Code::_400_BAD_REQUEST,
          {"InvalidRequest", "Please use AWS4-HMAC-SHA256."}}},
        {S3Error::Code::_400_AUTHORIZATION_HEADER_MALFORMED,
         {HttpError::Code::_400_BAD_REQUEST,
          {"AuthorizationHeaderMalformed",
           "The authorization header you provided is invalid."}}},
        {S3Error::Code::_403_INVALID_KEY_ID,
         {HttpError::Code::_403_FORBIDDEN,
          {"InvalidAccessKeyId", "The given key is not valid."}}},
        {S3Error::Code::_403_ACCESS_DENIED,
         {HttpError::Code::_403_FORBIDDEN, {"AccessDenied", "Access Denied"}}},
        {S3Error::Code::_403_SIGNATURE_DOES_NOT_MATCH,
         {HttpError::Code::_403_FORBIDDEN,
          {"SignatureDoesNotMatch",
           "The request signature we calculated does not match the signature you provided."}}},
        {S3Error::Code::_404_NO_SUCH_BUCKET,
         {HttpError::Code::_404_NOT_FOUND,
          {"NoSuchBucket", "The specified bucket does not exist."}}},
        {S3Error::Code::_404_NO_SUCH_KEY,
         {HttpError::Code::_404_NOT_FOUND,
          {"NoSuchKey", "The specified key does not exist."}}},
        {S3Error::Code::_404_NO_SUCH_VERSION,
         {HttpError::Code::_404_NOT_FOUND,
          {"NoSuchVersion",
           "The version ID specified in the request does not match any existing version."}}},
        {S3Error::Code::_404_NOT_IMPLEMENTED,
         {HttpError::Code::_404_NOT_FOUND,
          {"NotImplemented",
           "A header you provided implies functionality that is not implemented."}}},
        {S3Error::Code::_409_BUCKET_EXISTS,
         {HttpError::Code::_409_CONFLICT,
          {"BucketAlreadyExists",
           "The requested bucket name is not available. The bucket namespace is shared by all users of the system. Please select a different name and try again."}}},
        {S3Error::Code::_409_BUCKET_NOT_EMPTY,
         {HttpError::Code::_409_CONFLICT,
          {"BucketNotEmpty", "The bucket you tried to delete is not empty."}}},
        {S3Error::Code::_411_MISSING_CONTENT_LENGTH,
         {HttpError::Code::_411_LENGTH_REQURED,
          {"MissingContentLength",
           "You must provide the Content-Length HTTP header."}}},
        {S3Error::Code::_500_INTERNAL_SERVER_ERROR,
         {HttpError::Code::_400_BAD_REQUEST,
          {"InternalError",
           "We encountered an internal error. Please try again."}}},
};

S3Error::S3Error(
    Code code, const S3Request& request, const std::string& message_details) :
    HttpError(),
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
        m_code = HttpError::Code::_500_INTERNAL_SERVER_ERROR;
    }
}

void S3Error::from_xml(const XmlElement& element)
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

S3Error::S3Error(const HttpResponse& http_response)
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
        m_code = HttpError::get_code_from_numeric(http_response.code());
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

void S3Error::add_field(const std::string& key, const std::string& value)
{
    m_fields.push_back({key, value});
}

bool S3Error::is_ok() const
{
    return get_code_and_id().first < 400;
}

void S3Error::add_message_details(const std::string& details)
{
    m_message_details = details;
}

S3Error::Code S3Error::get_s3_code() const
{
    return m_s3_code;
}

XmlElementPtr S3Error::to_xml() const
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

std::string S3Error::to_string() const
{
    XmlDocument xml_doc;

    xml_doc.set_root(to_xml());
    return xml_doc.to_string();
}
}  // namespace hestia