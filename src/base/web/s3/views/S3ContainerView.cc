#include "S3ContainerView.h"

#include "S3Service.h"
#include "S3ViewUtils.h"

#include "S3Container.h"
#include "S3Path.h"
#include "XmlUtils.h"

#include <iostream>
#include <sstream>

namespace hestia {
S3ContainerView::S3ContainerView(S3Service* service) : m_service(service) {}

HttpResponse::Ptr S3ContainerView::on_get(const HttpRequest& request)
{
    const auto s3_path = S3Path(request.get_path());

    S3Container container(s3_path.m_container_name);

    CHECK_EXISTS(container);

    FAIL_CHECK(m_service->get(container));

    auto response = HttpResponse::create();
    S3ViewUtils::metadata_to_header(container.m_user_metadata, response.get());

    std::vector<S3Object> objects;
    FAIL_CHECK(m_service->list(container, objects));

    std::stringstream sstr;
    XmlUtils::add_default_prolog(sstr);
    XmlUtils::open_tag(sstr, "ListBucketResult");

    XmlUtils::add_tag_and_text(sstr, "Name", container.m_name);
    XmlUtils::add_tag_and_text(
        sstr, "KeyCount", std::to_string(objects.size()));

    for (const auto& object : objects) {
        XmlUtils::open_tag(sstr, "Contents");

        XmlUtils::add_tag_and_text(sstr, "Key", object.m_key);
        XmlUtils::add_tag_and_text(
            sstr, "LastModified", object.m_creation_time);
        XmlUtils::add_tag_and_text(sstr, "Size", object.m_content_length);

        XmlUtils::close_tag(sstr, "Contents");
    }
    XmlUtils::close_tag(sstr, "ListBucketResult");

    response->set_body(sstr.str());
    response->header().set_item(
        "Content-Length", std::to_string(response->body().size()));
    return response;
}

HttpResponse::Ptr S3ContainerView::on_put(const HttpRequest& request)
{
    const auto s3_path = S3Path(request.get_path());

    S3Container container(s3_path.m_container_name);
    FAIL_IF_EXISTS(container);

    S3ViewUtils::header_to_metadata(request, container.m_user_metadata);

    FAIL_CHECK(m_service->put(container));

    return HttpResponse::create(201, "Created");
}

HttpResponse::Ptr S3ContainerView::on_delete(const HttpRequest& request)
{
    const auto s3_path = S3Path(request.get_path());
    FAIL_CHECK(m_service->remove(S3Container(s3_path.m_container_name)));
    return {};
}

HttpResponse::Ptr S3ContainerView::on_head(const HttpRequest& request)
{
    const auto s3_path = S3Path(request.get_path());
    S3Container container(s3_path.m_container_name);

    CHECK_EXISTS(container);

    FAIL_CHECK(m_service->get(container));

    auto response = HttpResponse::create();
    S3ViewUtils::metadata_to_header(container.m_user_metadata, response.get());
    return response;
}
}  // namespace hestia