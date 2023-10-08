#include "S3ViewUtils.h"

#include "S3Request.h"
#include "S3Status.h"

namespace hestia {

CrudIdentifier S3ViewUtils::path_to_crud_id(const S3Path& path)
{
    CrudIdentifier id;
    id.set_name(path.m_object_key);
    id.set_parent_name(path.m_bucket_name);
    return id;
}

HttpResponse::Ptr S3ViewUtils::on_server_error(
    const S3Request& req, const std::string& msg)
{
    auto response =
        HttpResponse::create({HttpStatus::Code::_500_INTERNAL_SERVER_ERROR});
    S3Status s3_status(S3StatusCode::_500_INTERNAL_SERVER_ERROR, req, msg);
    response->set_body(s3_status.to_string());
    set_common_headers(*response);
    return response;
}

HttpResponse::Ptr S3ViewUtils::on_no_such_bucket(const S3Request& req)
{
    auto response = HttpResponse::create(HttpStatus::Code::_404_NOT_FOUND);
    S3Status s3_error(S3StatusCode::_404_NO_SUCH_BUCKET, req);
    response->set_body(s3_error.to_string());
    set_common_headers(*response);
    return response;
}

HttpResponse::Ptr S3ViewUtils::on_no_such_key(
    const S3Request& req, bool no_bucket)
{
    auto response = HttpResponse::create(HttpStatus::Code::_404_NOT_FOUND);
    S3Status s3_error(S3StatusCode::_404_NO_SUCH_KEY, req);
    if (no_bucket) {
        s3_error.add_message_details("Couldn't find bucket.");
    }
    response->set_body(s3_error.to_string());
    set_common_headers(*response);
    return response;
}

HttpResponse::Ptr S3ViewUtils::on_bucket_already_exists(const S3Request& req)
{
    auto response = HttpResponse::create(HttpStatus::Code::_409_CONFLICT);
    S3Status s3_error(S3StatusCode::_409_BUCKET_EXISTS, req);
    response->set_body(s3_error.to_string());
    set_common_headers(*response);
    return response;
}

HttpResponse::Ptr S3ViewUtils::on_tried_to_delete_nonempty_bucket(
    const S3Request& req)
{
    auto response = HttpResponse::create(HttpStatus::Code::_409_CONFLICT);
    S3Status s3_error(S3StatusCode::_409_BUCKET_NOT_EMPTY, req);
    response->set_body(s3_error.to_string());
    set_common_headers(*response);
    return response;
}

void S3ViewUtils::set_common_headers(HttpResponse& response)
{
    response.header().set_item(
        "Content-Length", std::to_string(response.body().size()));
    response.header().set_content_type("application/xml");
}

void S3ViewUtils::metadata_to_header(
    const Map& metadata, HttpResponse* response)
{
    auto on_item =
        [&response](const std::string& key, const std::string& value) {
            response->header().set_item(S3Path::meta_prefix + key, value);
        };
    metadata.for_each_item(on_item);
}

void S3ViewUtils::header_to_metadata(const HttpRequest& request, Map& metadata)
{
    metadata = request.get_header().get_items_with_prefix(S3Path::meta_prefix);
}
}  // namespace hestia