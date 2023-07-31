#include "CrudWebView.h"

#include "CrudWebPages.h"
#include "JsonUtils.h"

#include <iostream>

namespace hestia {

std::string CrudWebView::get_path(const HttpRequest& request) const
{
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/" + m_type_name + "s")
            .second;
    return hestia::StringUtils::remove_prefix(path, "/");
}

HttpResponse::Ptr CrudWebView::on_get(
    const HttpRequest& request, const User& user)
{
    const auto path = get_path(request);
    auto response   = hestia::HttpResponse::create();
    if (path.empty()) {
        if (request.get_header().has_html_accept_type()) {
            CrudQuery query(CrudQuery::OutputFormat::DICT);
            auto crud_response = m_service->make_request(
                CrudRequest(query, user.get_primary_key()), m_type_name);

            std::string json_body;
            JsonUtils::to_json(*crud_response->dict(), json_body, {}, 4);
            response->set_body(
                CrudWebPages::get_item_view(m_type_name, json_body));
            response->header().set_content_type("text/html");
        }
        else {
            CrudQuery query(CrudQuery::OutputFormat::ATTRIBUTES);
            auto crud_response = m_service->make_request(
                CrudRequest(query, user.get_primary_key()), m_type_name);
            response->set_body(crud_response->attributes().get_buffer());
            response->header().set_content_type("application/json");
        }
    }
    else {
        const auto id = path;
        CrudQuery query(
            CrudIdentifier(id), CrudQuery::OutputFormat::ATTRIBUTES);
        auto crud_response = m_service->make_request(
            CrudRequest{query, user.get_primary_key()}, m_type_name);
        response->set_body(crud_response->attributes().get_buffer());
    }
    return response;
}

HttpResponse::Ptr CrudWebView::on_put(
    const HttpRequest& request, const User& user)
{
    const auto path = get_path(request);

    auto response = HttpResponse::create();

    CrudAttributes attributes;

    attributes.set_buffer(request.body());

    CrudResponse::Ptr crud_response;
    if (path.empty()) {
        crud_response = m_service->make_request(
            CrudRequest{
                CrudMethod::CREATE,
                user.get_primary_key(),
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
                user.get_primary_key(),
                {id},
                attributes,
                CrudQuery::OutputFormat::ATTRIBUTES},
            m_type_name);
    }

    response->set_body(crud_response->attributes().get_buffer());
    return response;
}
}  // namespace hestia