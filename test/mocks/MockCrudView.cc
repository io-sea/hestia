#include "MockCrudView.h"

#include "Logger.h"
#include "MockModel.h"

#include <iostream>

namespace hestia::mock {

std::string MockCrudView::get_path(const HttpRequest& request) const
{
    const auto path = StringUtils::split_on_first(
                          request.get_path(), "/" + m_service->get_type() + "s")
                          .second;
    return hestia::StringUtils::remove_prefix(path, "/");
}

HttpResponse::Ptr MockCrudView::on_get(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    const auto path = get_path(request);
    auto response   = hestia::HttpResponse::create();
    if (path.empty()) {
        CrudQuery query(CrudQuery::BodyFormat::JSON);
        auto crud_response =
            m_service->make_request(CrudRequest(CrudMethod::READ, query, auth));

        CrudQuery::FormatSpec output_format;
        output_format.m_attrs_format.set_is_json();
        crud_response->write(response->body(), output_format);
    }
    else {
        const auto id = path;
        CrudQuery query(CrudIdentifier(id), CrudQuery::BodyFormat::JSON);
        auto crud_response =
            m_service->make_request(CrudRequest{CrudMethod::READ, query, auth});

        CrudQuery::FormatSpec output_format;
        output_format.m_attrs_format.set_is_json();
        crud_response->write(response->body(), output_format);
    }
    return response;
}

HttpResponse::Ptr MockCrudView::on_put(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    LOG_INFO("Starting PUT");
    const auto path = get_path(request);

    auto response = hestia::HttpResponse::create();

    CrudAttributes::FormatSpec format;
    format.set_is_json();

    CrudResponse::Ptr crud_response;
    if (path.empty()) {
        crud_response = m_service->make_request(CrudRequest{
            CrudMethod::CREATE,
            {request.body(), format, CrudQuery::BodyFormat::JSON},
            auth});
    }
    else {
        crud_response = m_service->make_request(CrudRequest{
            CrudMethod::UPDATE,
            {CrudIdentifier(path), CrudAttributes(request.body(), format),
             CrudQuery::BodyFormat::JSON},
            auth});
    }

    CrudQuery::FormatSpec output_format;
    output_format.m_attrs_format.set_is_json();
    crud_response->write(response->body(), output_format);
    LOG_INFO("Finished PUT: ");
    return response;
}
}  // namespace hestia::mock