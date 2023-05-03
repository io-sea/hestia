#include "S3Error.h"

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

S3Error::S3Error(Code code, const std::string& request) :
    HttpError(), m_request(request)
{
    if (const auto iter = errors.find(code); iter != errors.end()) {
        m_code       = iter->second.first;
        m_identifier = iter->second.second.first;
        m_message    = iter->second.second.second;
    }
    else {
        m_code = HttpError::Code::_500_INTERNAL_SERVER_ERROR;
    }
}

std::string S3Error::to_string() const
{
    // const auto& [code, _] = getCodeAndId();

    std::stringstream sstr;
    sstr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    sstr << "<Error>\n";
    sstr << "<Code>" << m_identifier << "</Code>\n";
    sstr << "<Message>" << m_message << "</Message>\n";
    sstr << "<Resource>/" << m_request << "</Resource>\n";
    sstr << "</Error>";

    return sstr.str();
}
}  // namespace hestia