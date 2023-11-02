#include "HttpCrudClient.h"

#include "RequestError.h"
#include "RequestException.h"

#include "ErrorUtils.h"
#include "HttpClient.h"
#include "HttpCrudPath.h"

#include "Logger.h"

#include <iostream>
#include <sstream>

namespace hestia {
HttpCrudClient::HttpCrudClient(
    const CrudClientConfig& config,
    ModelSerializer::Ptr serializer,
    HttpClient* client) :
    CrudClient(config, std::move(serializer)), m_client(client)
{
}

HttpCrudClient::~HttpCrudClient() {}

void apply_common_headers(
    const CrudRequest& crud_request, HttpRequest& http_request)
{
    http_request.get_header().set_content_type("application/json");
    http_request.get_header().set_auth_token(
        crud_request.get_user_context().m_token);
}

HttpResponse::Ptr HttpCrudClient::make_request(const HttpRequest& req) const
{
    auto response = m_client->make_request(req);
    if (response->error()) {
        throw RequestException<CrudRequestError>(
            {CrudErrorCode::ERROR, SOURCE_LOC() + " | Error in http client: "
                                       + response->to_string()});
    }
    return response;
}

void HttpCrudClient::make_request(
    const CrudRequest& crud_request,
    HttpRequest::Method method,
    CrudResponse& crud_response) const
{
    if (crud_request.has_ids() && crud_request.method() != CrudMethod::CREATE) {
        std::size_t count{0};
        for (const auto& id : crud_request.get_ids().data()) {
            std::string path_suffix;
            HttpCrudPath::from_identifier(id, path_suffix);

            HttpRequest req(get_item_path() + path_suffix, method);
            apply_common_headers(crud_request, req);

            m_serializer->to_json(crud_request, req.body(), count);

            const auto response = make_request(req);

            m_serializer->append_json(response->body(), crud_response);
            count++;
        }
    }
    else {
        HttpRequest req(get_item_path(), method);
        apply_common_headers(crud_request, req);

        m_serializer->to_json(crud_request, req.body());

        const auto response = make_request(req);

        m_serializer->append_json(response->body(), crud_response);
    }
}

void HttpCrudClient::create(
    const CrudRequest& crud_request, CrudResponse& crud_response, bool)
{
    make_request(crud_request, HttpRequest::Method::PUT, crud_response);
}

void HttpCrudClient::update(
    const CrudRequest& crud_request, CrudResponse& crud_response, bool) const
{
    make_request(crud_request, HttpRequest::Method::PUT, crud_response);
}

void HttpCrudClient::remove(
    const CrudRequest& crud_request, CrudResponse& crud_response) const
{
    make_request(crud_request, HttpRequest::Method::DELETE, crud_response);
}

void HttpCrudClient::read(
    const CrudRequest& crud_request, CrudResponse& crud_response) const
{
    if (crud_request.get_query().is_id()
        && crud_request.get_query().get_ids().size() != 1) {
        make_request(crud_request, HttpRequest::Method::GET, crud_response);
    }
    else {
        std::string path = get_item_path();
        HttpCrudPath::from_query(crud_request.get_query(), path);

        HttpRequest request(path, HttpRequest::Method::GET);
        apply_common_headers(crud_request, request);

        const auto response = make_request(request);

        m_serializer->append_json(response->body(), crud_response);
    }
}

void HttpCrudClient::identify(
    const CrudRequest& request, CrudResponse& response) const
{
    (void)request;
    (void)response;
}

void HttpCrudClient::lock(
    const CrudIdentifier& id, CrudLockType lock_type) const
{
    (void)id;
    (void)lock_type;
}

void HttpCrudClient::unlock(
    const CrudIdentifier& id, CrudLockType lock_type) const
{
    (void)id;
    (void)lock_type;
}

bool HttpCrudClient::is_locked(
    const CrudIdentifier& id, CrudLockType lock_type) const
{
    (void)id;
    (void)lock_type;
    return false;
}

std::string HttpCrudClient::get_item_path(const CrudIdentifier& id) const
{
    if (id.has_primary_key()) {
        return get_item_path(id.get_primary_key());
    }
    else {
        std::string queries;
        if (id.has_parent_primary_key()) {
            queries += "parent_id=" + id.get_parent_primary_key();
        }
        auto path = get_item_path();
        if (!queries.empty()) {
            path += "/?" + queries;
        }
        return path;
    }
}

std::string HttpCrudClient::get_item_path(const std::string& id) const
{
    auto path = m_config.m_endpoint + "/" + get_type() + "s";
    if (!id.empty()) {
        path += "/" + id;
    }
    return path;
}

void HttpCrudClient::get_item_keys(
    const std::vector<std::string>& ids, std::vector<std::string>& keys) const
{
    for (const auto& id : ids) {
        keys.push_back(m_config.m_prefix + ":" + get_type() + ":" + id);
    }
}

std::string HttpCrudClient::get_set_key() const
{
    return m_config.m_prefix + ":" + get_type() + "s";
}
}  // namespace hestia