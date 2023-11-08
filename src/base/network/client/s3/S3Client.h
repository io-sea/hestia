#pragma once

#include "HttpClient.h"
#include "S3Bucket.h"
#include "S3Object.h"
#include "S3Path.h"
#include "S3Responses.h"
#include "S3Status.h"
#include "S3Types.h"

#include "HttpRequest.h"
#include "S3ListObjectsRequest.h"
#include "S3Request.h"

#include <functional>

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

    using completionFunc = std::function<void(S3Response::Ptr response)>;
    void get_object(
        const S3Object& object,
        const S3Bucket& bucket,
        const S3Request& request,
        Stream* stream,
        completionFunc completion_func,
        std::size_t progress_interval          = 0,
        HttpClient::progressFunc progress_func = nullptr);

    void put_object(
        const S3Object& object,
        const S3Bucket& bucket,
        const S3Request& request,
        Stream* stream,
        completionFunc completion_func,
        std::size_t progress_interval          = 0,
        HttpClient::progressFunc progress_func = nullptr);

    S3ListBucketResponse::Ptr list_buckets(const S3Request& request) const;

    S3ListObjectsResponse::Ptr list_objects(
        const S3Bucket& bucket, const S3ListObjectsRequest& request);

  private:
    void prepare_headers(
        const S3Request& request,
        const S3Bucket& bucket,
        HttpHeader& header,
        const std::string& host = {}) const;

    S3Response::Ptr do_request(const HttpRequest& request) const;

    HttpClient* m_http_client{nullptr};
};
}  // namespace hestia