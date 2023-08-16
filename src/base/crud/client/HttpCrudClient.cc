#include "HttpCrudClient.h"

#include "RequestError.h"
#include "RequestException.h"

#include "HttpClient.h"
#include "HttpCrudPath.h"
#include "StringAdapter.h"

#include "Logger.h"

#include <iostream>
#include <sstream>

namespace hestia {
HttpCrudClient::HttpCrudClient(
    const CrudClientConfig& config,
    AdapterCollectionPtr adapters,
    HttpClient* client) :
    CrudClient(config, std::move(adapters)), m_client(client)
{
}

HttpCrudClient::~HttpCrudClient() {}

void HttpCrudClient::create(
    const CrudRequest& crud_request, CrudResponse& crud_response, bool)
{
    const auto path = m_config.m_endpoint + "/" + m_adapters->get_type() + "s";
    HttpRequest request(path, HttpRequest::Method::PUT);
    request.get_header().set_content_type("application/json");
    request.get_header().set_auth_token(
        crud_request.get_user_context().m_token);

    if (crud_request.has_items()) {
        get_adapter(CrudAttributes::Format::JSON)
            ->to_string(crud_request.items(), request.body());
    }

    const auto response = m_client->make_request(request);
    if (response->error()) {
        throw RequestException<CrudRequestError>(
            {CrudErrorCode::ERROR,
             "Error in http client CREATE: " + response->to_string()});
    }

    get_adapter(CrudAttributes::Format::JSON)
        ->from_string({response->body()}, crud_response.items());

    std::vector<std::string> ids;
    for (const auto& item : crud_response.items()) {
        ids.push_back({item->get_primary_key()});
    }
    crud_response.ids() = ids;

    if (crud_request.get_query().is_attribute_output_format()) {
        crud_response.attributes().buffer() = response->body();
    }
    else if (crud_request.get_query().is_dict_output_format()) {
        auto content = std::make_unique<Dictionary>();
        get_adapter(CrudAttributes::Format::JSON)
            ->dict_from_string(response->body(), *content);
        crud_response.set_dict(std::move(content));
    }
}

void HttpCrudClient::read(
    const CrudRequest& crud_request, CrudResponse& crud_response) const
{
    std::string path =
        m_config.m_endpoint + "/" + m_adapters->get_type() + "s/";

    if (crud_request.get_query().is_id()
        && !crud_request.get_query().has_single_id()) {
        Dictionary dict(Dictionary::Type::SEQUENCE);
        for (const auto& id : crud_request.get_query().ids()) {
            std::string path_suffix;
            HttpCrudPath::from_identifier(id, path_suffix);
            HttpRequest request(path + path_suffix, HttpRequest::Method::GET);
            request.get_header().set_content_type("application/json");
            request.get_header().set_auth_token(
                crud_request.get_user_context().m_token);

            const auto response = m_client->make_request(request);

            if (response->error()) {
                if (response->code() == 404) {
                    return;
                }
                throw RequestException<CrudRequestError>(
                    {CrudErrorCode::ERROR,
                     "Error in http client GET: " + response->to_string()});
            }

            auto item_dict = std::make_unique<Dictionary>();
            get_adapter(CrudAttributes::Format::JSON)
                ->dict_from_string(request.body(), *item_dict);
            dict.add_sequence_item(std::move(item_dict));
        }
        get_adapter(CrudAttributes::Format::JSON)
            ->from_dict(dict, crud_response.items());
    }
    else {
        HttpCrudPath::from_query(crud_request.get_query(), path);

        HttpRequest request(path, HttpRequest::Method::GET);
        request.get_header().set_content_type("application/json");
        request.get_header().set_auth_token(
            crud_request.get_user_context().m_token);

        const auto response = m_client->make_request(request);

        if (response->error()) {
            if (response->code() == 404) {
                return;
            }
            throw RequestException<CrudRequestError>(
                {CrudErrorCode::ERROR,
                 "Error in http client GET: " + response->to_string()});
        }
        LOG_INFO("Got response: " << response->body());

        std::vector<std::string> ids;

        if (crud_request.get_query().is_id_output_format()) {
            Dictionary dict;
            get_adapter(CrudAttributes::Format::JSON)
                ->dict_from_string(response->body(), dict);
            if (dict.get_type() == Dictionary::Type::SEQUENCE) {
                for (const auto& item : dict.get_sequence()) {
                    if (item->has_map_item("id")) {
                        ids.push_back(item->get_map_item("id")->get_scalar());
                    }
                }
            }
            else {
                if (dict.has_map_item("id")) {
                    ids.push_back(dict.get_map_item("id")->get_scalar());
                }
            }
            crud_response.ids() = ids;
        }
        else if (crud_request.get_query().is_attribute_output_format()) {
            crud_response.attributes().buffer() = response->body();
        }
        else if (crud_request.get_query().is_dict_output_format()) {
            auto dict = std::make_unique<Dictionary>();
            get_adapter(CrudAttributes::Format::JSON)
                ->dict_from_string(response->body(), *dict);
            crud_response.set_dict(std::move(dict));
        }
        else if (crud_request.get_query().is_item_output_format()) {
            get_adapter(CrudAttributes::Format::JSON)
                ->from_string({response->body()}, crud_response.items());
        }
    }
}

void HttpCrudClient::update(
    const CrudRequest& crud_request, CrudResponse& crud_response, bool) const
{
    auto path = m_config.m_endpoint + "/" + m_adapters->get_type() + "s";

    if (crud_request.has_items()) {
        if (crud_request.items().size() == 1) {
            path += "/" + crud_request.items()[0]->get_primary_key();

            HttpRequest request(path, HttpRequest::Method::PUT);
            request.get_header().set_content_type("application/json");
            request.get_header().set_auth_token(
                crud_request.get_user_context().m_token);

            get_adapter(CrudAttributes::Format::JSON)
                ->to_string(crud_request.items(), request.body());

            const auto response = m_client->make_request(request);
            if (response->error()) {
                throw RequestException<CrudRequestError>(
                    {CrudErrorCode::ERROR,
                     "Error in http client PUT: " + response->to_string()});
            }
            get_adapter(CrudAttributes::Format::JSON)
                ->from_string({response->body()}, crud_response.items());
        }
    }
}

void HttpCrudClient::remove(const VecCrudIdentifier& ids) const
{
    (void)ids;
    /*
    const auto string_response = m_client->make_request(
        {KeyValueStoreRequestMethod::STRING_REMOVE, get_item_key(item),
            m_config.m_endpoint});
    if (!string_response->ok()) {
        throw std::runtime_error(
            "Error in kv_store STRING_REMOVE: "
            + string_response->get_error().to_string());
    }

    const auto set_response = m_client->make_request(
        {KeyValueStoreRequestMethod::SET_REMOVE,
            Metadata::Query(get_set_key(), item.id()), m_config.m_endpoint});
    if (!set_response->ok()) {
        throw std::runtime_error(
            "Error in kv_store SET_REMOVE: "
            + set_response->get_error().to_string());
    }
    */
}

void HttpCrudClient::identify(
    const VecCrudIdentifier& ids, CrudResponse& response) const
{
    (void)ids;
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

std::string HttpCrudClient::get_item_path(const std::string& id) const
{
    return m_config.m_prefix + ":" + m_adapters->get_type() + ":" + id;
}

void HttpCrudClient::get_item_keys(
    const std::vector<std::string>& ids, std::vector<std::string>& keys) const
{
    for (const auto& id : ids) {
        keys.push_back(
            m_config.m_prefix + ":" + m_adapters->get_type() + ":" + id);
    }
}

std::string HttpCrudClient::get_set_key() const
{
    return m_config.m_prefix + ":" + m_adapters->get_type() + "s";
}
}  // namespace hestia