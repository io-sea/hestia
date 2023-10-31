#include "CrudWebView.h"

#include "CrudWebPages.h"
#include "JsonUtils.h"

#include <iostream>

namespace hestia {

CrudWebView::CrudWebView(CrudService* service, const std::string& type_name) :
    WebView(), m_service(service), m_type_name(type_name)
{
    m_readable_format.m_attrs_format.set_is_json();
    m_readable_format.m_attrs_format.m_json_format.m_indent      = true;
    m_readable_format.m_attrs_format.m_json_format.m_indent_size = 4;

    m_machine_format.m_attrs_format.set_is_json();
}

std::string CrudWebView::get_path(const HttpRequest& request) const
{
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/" + m_type_name + "s")
            .second;
    return hestia::StringUtils::remove_prefix(path, "/");
}

HttpResponse::Ptr CrudWebView::on_get(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::EOM) {
        return HttpResponse::create(
            HttpResponse::CompletionStatus::AWAITING_EOM);
    }

    const auto path         = get_path(request);
    auto response           = hestia::HttpResponse::create();
    const auto content_type = request.get_header().has_html_accept_type() ?
                                  "text/html" :
                                  "application/json";
    response->header().set_content_type(content_type);

    if (path.empty()) {
        const auto response_type = request.get_header().has_html_accept_type() ?
                                       CrudQuery::BodyFormat::DICT :
                                       CrudQuery::BodyFormat::JSON;
        CrudQuery query(response_type);
        if (!request.get_queries().empty()) {
            CrudIdentifier id(
                request.get_queries(), CrudIdentifier::FormatSpec());
            if (id.has_value()) {

                query.ids().add(id);
            }
        }

        auto crud_response = m_service->make_request(
            CrudRequest(CrudMethod::READ, query, auth), m_type_name);
        if (!crud_response->ok()) {
            return HttpResponse::create(
                {HttpStatus::Code::_500_INTERNAL_SERVER_ERROR,
                 crud_response->get_error().to_string()});
        }

        if (!query.get_ids().empty() && !crud_response->found()) {
            return HttpResponse::create({HttpStatus::Code::_404_NOT_FOUND});
        }

        if (request.get_header().has_html_accept_type()) {
            std::string json_body;
            crud_response->write(json_body, m_readable_format);
            response->set_body(
                CrudWebPages::get_item_view(m_type_name, json_body));
        }
        else {
            crud_response->write(response->body(), m_machine_format);
        }
    }
    else {
        const auto id = path;
        CrudQuery query(id, CrudQuery::BodyFormat::JSON);
        auto crud_response = m_service->make_request(
            CrudRequest{CrudMethod::READ, query, auth}, m_type_name);
        if (!crud_response->ok()) {
            return HttpResponse::create(
                {HttpStatus::Code::_500_INTERNAL_SERVER_ERROR,
                 crud_response->get_error().to_string()});
        }
        if (!crud_response->found()) {
            return HttpResponse::create({HttpStatus::Code::_404_NOT_FOUND});
        }
        crud_response->write(response->body(), m_machine_format);
    }
    return response;
}

HttpResponse::Ptr CrudWebView::on_delete(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::EOM) {
        return HttpResponse::create(
            HttpResponse::CompletionStatus::AWAITING_EOM);
    }

    const auto path = get_path(request);
    if (path.empty()) {
        return HttpResponse::create(HttpStatus::Code::_400_BAD_REQUEST);
    }

    auto crud_response = m_service->make_request(
        CrudRequest{CrudMethod::REMOVE, {CrudIdentifier(path)}, auth},
        m_type_name);
    if (!crud_response->ok()) {
        return HttpResponse::create(
            {HttpStatus::Code::_500_INTERNAL_SERVER_ERROR,
             crud_response->get_error().to_string()});
    }
    if (!crud_response->found()) {
        return HttpResponse::create({HttpStatus::Code::_404_NOT_FOUND});
    }
    return HttpResponse::create(HttpStatus::Code::_204_NO_CONTENT);
}

HttpResponse::Ptr CrudWebView::on_put(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::EOM) {
        return HttpResponse::create(
            HttpResponse::CompletionStatus::AWAITING_EOM);
    }

    const auto path = get_path(request);

    auto response = HttpResponse::create();

    CrudAttributes attributes(JsonDocument::create(request.body()));

    const std::string parent_id = request.get_queries().get_item("parent_id");

    CrudResponse::Ptr crud_response;
    if (path.empty() && parent_id.empty()) {
        crud_response = m_service->make_request(
            CrudRequest{
                CrudMethod::CREATE,
                {attributes, CrudQuery::BodyFormat::JSON},
                auth},
            m_type_name);
    }
    else {
        CrudIdentifier id;
        if (!parent_id.empty()) {
            id.set_parent_primary_key(parent_id);
        }
        else {
            id.set_primary_key(path);
        }
        crud_response = m_service->make_request(
            CrudRequest{
                CrudMethod::UPDATE,
                {id, attributes, CrudQuery::BodyFormat::JSON},
                auth},
            m_type_name);
    }
    if (!crud_response->ok()) {
        return HttpResponse::create(
            {HttpStatus::Code::_500_INTERNAL_SERVER_ERROR,
             crud_response->get_error().to_string()});
    }
    crud_response->write(response->body(), m_machine_format);
    return response;
}
}  // namespace hestia