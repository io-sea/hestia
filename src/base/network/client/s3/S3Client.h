#pragma once

#include "HttpClient.h"
#include "S3Bucket.h"
#include "S3Object.h"
#include "S3Path.h"
#include "S3Responses.h"
#include "S3Status.h"

#include "HttpRequest.h"
#include "S3Request.h"

namespace hestia {

class S3Client {
  public:
    using Ptr = std::unique_ptr<S3Client>;
    S3Client(HttpClient* http_client);

    ~S3Client();

    S3Status create_bucket(
        const S3Bucket& bucket, const S3Request& request) const;

    S3Status delete_bucket(
        const S3Bucket& bucket, const S3Request& request) const;

    S3Status head_bucket(
        const S3Bucket& bucket, const S3Request& request) const;

    S3Status delete_object(
        const S3Object& object,
        const S3Bucket& bucket,
        const S3Request& request) const;

    S3Status get_object(
        const S3Object& object,
        const S3Bucket& bucket,
        const S3Request& request,
        Stream* stream);

    S3ListBucketResponse::Ptr list_buckets(const S3Request& request) const;

    S3Status put_object(
        const S3Object& object,
        const S3Bucket& bucket,
        const S3Request& request,
        Stream* stream);

    S3ListObjectsResponse::Ptr list_objects(
        const S3Bucket& bucket, const S3ListObjectRequest& request);

  private:
    std::string get_authorization_header(
        const S3UserContext& user_context, const HttpRequest& request) const;

    void prepare_headers(HttpHeader& header, const std::string& path) const;

    S3Response::Ptr do_request(const HttpRequest& request) const;

    HttpClient* m_http_client{nullptr};
};
}  // namespace hestia