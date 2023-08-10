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
    const auto path         = get_path(request);
    auto response           = hestia::HttpResponse::create();
    const auto content_type = request.get_header().has_html_accept_type() ?
                                  "text/html" :
                                  "application/json";
    response->header().set_content_type(content_type);

    std::string token;
    if (!user.tokens().empty()) {
        token = user.tokens()[0].value();
    }

    if (path.empty()) {

        const auto query_type = request.get_header().has_html_accept_type() ?
                                    CrudQuery::OutputFormat::DICT :
                                    CrudQuery::OutputFormat::ATTRIBUTES;
        CrudQuery query(query_type);
        bool has_id{false};
        if (!request.get_queries().empty()) {
            CrudIdentifier id;
            if (const auto name_val = request.get_queries().get_item("name");
                !name_val.empty()) {
                id.set_name(name_val);
                has_id = true;
            }
            if (const auto parent_name_val =
                    request.get_queries().get_item("parent_name");
                !parent_name_val.empty()) {
                id.set_parent_name(parent_name_val);
            }
            if (const auto parent_id_val =
                    request.get_queries().get_item("parent_id");
                !parent_id_val.empty()) {
                id.set_parent_primary_key(parent_id_val);
            }

            if (has_id) {
                query.set_ids({id});
            }
        }

        auto crud_response = m_service->make_request(
            CrudRequest(query, {user.get_primary_key(), token}), m_type_name);

        if (has_id && !crud_response->found()) {
            return HttpResponse::create({HttpError::Code::_404_NOT_FOUND});
        }

        if (request.get_header().has_html_accept_type()) {
            std::string json_body;
            JsonUtils::to_json(*crud_response->dict(), json_body, {}, 4);
            response->set_body(
                CrudWebPages::get_item_view(m_type_name, json_body));
        }
        else {
            response->set_body(crud_response->attributes().get_buffer());
        }
    }
    else {
        const auto id = path;
        CrudQuery query(
            CrudIdentifier(id), CrudQuery::OutputFormat::ATTRIBUTES);
        auto crud_response = m_service->make_request(
            CrudRequest{query, {user.get_primary_key(), token}}, m_type_name);
        if (!crud_response->found()) {
            return HttpResponse::create({HttpError::Code::_404_NOT_FOUND});
        }
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
    if (!crud_response->ok()) {
        return HttpResponse::create(
            {HttpError::Code::_500_INTERNAL_SERVER_ERROR,
             crud_response->get_error().to_string()});
    }

    response->set_body(crud_response->attributes().get_buffer());
    return response;
}
}  // namespace hestia