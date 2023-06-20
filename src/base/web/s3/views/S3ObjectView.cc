#include "S3ObjectView.h"

#include "S3Service.h"
#include "S3ViewUtils.h"

#include "RequestContext.h"
#include "S3Object.h"
#include "S3Path.h"

#include "Logger.h"

#include <iostream>
#include <sstream>

namespace hestia {
S3ObjectView::S3ObjectView(S3Service* service) : m_service(service)
{

    LOG_INFO("Loaded object view");
}

HttpResponse::Ptr S3ObjectView::on_get(const HttpRequest& request, const User&)
{
    const auto s3_path = S3Path(request.get_path());

    S3Object object(s3_path.m_object_id);

    CHECK_EXISTS(object);

    FAIL_CHECK(m_service->get(object, {}, request.get_context()->get_stream()));

    auto response = HttpResponse::create();
    response->header().set_item(
        std::string(S3Path::meta_prefix) + "Creation-Time",
        object.m_creation_time);
    response->header().set_item(
        std::string(S3Path::meta_prefix) + "Bucket", object.m_container);
    S3ViewUtils::metadata_to_header(object.m_user_metadata, response.get());
    return response;
}

HttpResponse::Ptr S3ObjectView::on_head(const HttpRequest& request, const User&)
{
    const auto s3_path = S3Path(request.get_path());

    S3Object object(s3_path.m_object_id);

    CHECK_EXISTS(object);

    FAIL_CHECK(m_service->get(object));

    auto response = HttpResponse::create();
    response->header().set_item(
        std::string(S3Path::meta_prefix) + "Creation-Time",
        object.m_creation_time);
    response->header().set_item(
        std::string(S3Path::meta_prefix) + "Bucket", object.m_container);

    S3ViewUtils::metadata_to_header(object.m_user_metadata, response.get());
    return response;
}

HttpResponse::Ptr S3ObjectView::on_put(const HttpRequest& request, const User&)
{
    const auto s3_path = S3Path(request.get_path());

    S3Object object(s3_path.m_object_id);
    object.m_user_metadata =
        request.get_header().get_items_with_prefix(S3Path::meta_prefix);

    const auto content_length = request.get_header().get_content_length();
    object.m_content_length   = content_length.empty() ? "0" : content_length;

    S3Container container(s3_path.m_container_name);
    FAIL_CHECK(m_service->put(
        container, object, {}, request.get_context()->get_stream()));
    return HttpResponse::create(201, "Created");
}

HttpResponse::Ptr S3ObjectView::on_delete(
    const HttpRequest& request, const User&)
{
    const auto s3_path = S3Path(request.get_path());

    S3Object object(s3_path.m_object_id);

    CHECK_EXISTS(object);

    FAIL_CHECK(m_service->remove(object));
    return HttpResponse::create();
}
}  // namespace hestia