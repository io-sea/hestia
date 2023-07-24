#include "CrudWebView.h"

#include <iostream>

namespace hestia {

std::string CrudWebView::get_path(const HttpRequest& request) const
{
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/" + m_type_name + "s")
            .second;
    return hestia::StringUtils::remove_prefix(path, "/");
}

HttpResponse::Ptr CrudWebView::on_get(const HttpRequest& request, const User&)
{
    const auto path = get_path(request);
    auto response   = hestia::HttpResponse::create();
    if (path.empty()) {
        CrudQuery query(CrudQuery::OutputFormat::ATTRIBUTES);
        auto crud_response =
            m_service->make_request(CrudRequest(query), m_type_name);
        response->set_body(crud_response->attributes().get_buffer());
    }
    else {
        const auto id = path;
        CrudQuery query(
            CrudIdentifier(id), CrudQuery::OutputFormat::ATTRIBUTES);
        auto crud_response =
            m_service->make_request(CrudRequest{query}, m_type_name);
        response->set_body(crud_response->attributes().get_buffer());
    }
    return response;
}

HttpResponse::Ptr CrudWebView::on_put(const HttpRequest& request, const User&)
{
    const auto path = get_path(request);

    auto response = hestia::HttpResponse::create();

    CrudAttributes attributes;
    attributes.set_buffer(request.body());

    CrudResponse::Ptr crud_response;
    if (path.empty()) {
        crud_response = m_service->make_request(
            CrudRequest{
                CrudMethod::CREATE,
                {},
                attributes,
                CrudQuery::OutputFormat::ATTRIBUTES},
            m_type_name);
    }
    else {
        const CrudIdentifier id(path);
        crud_response = m_service->make_request(
            CrudRequest{
                CrudMethod::UPDATE,
                {id},
                attributes,
                CrudQuery::OutputFormat::ATTRIBUTES},
            m_type_name);
    }

    response->set_body(crud_response->attributes().get_buffer());
    return response;
}
}  // namespace hestia