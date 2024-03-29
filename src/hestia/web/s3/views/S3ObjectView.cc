#include "S3ObjectView.h"

#include "DistributedHsmService.h"
#include "S3HsmObjectAdapter.h"
#include "S3Path.h"
#include "S3ViewUtils.h"

#include "Dataset.h"
#include "HsmObject.h"
#include "TypedCrudRequest.h"

#include "HsmService.h"
#include "RequestContext.h"

#include "Logger.h"

#include <chrono>
#include <sstream>
#include <thread>

namespace hestia {
S3ObjectView::S3ObjectView(
    DistributedHsmService* service, const std::string& domain) :
    S3WebView(service, domain),
    m_object_adatper(std::make_unique<S3HsmObjectAdapter>())
{
    LOG_INFO("Loaded object view");
}

HttpResponse::Ptr S3ObjectView::on_get(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::HEADERS) {
        LOG_INFO("Returning empty 200");
        return HttpResponse::create();
    }
    LOG_INFO("Returning get or head");
    return on_get_or_head(request, event, auth, true);
}

HttpResponse::Ptr S3ObjectView::on_head(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::HEADERS) {
        return HttpResponse::create();
    }
    return on_get_or_head(request, event, auth, false);
}

HttpResponse::Ptr S3ObjectView::on_get_or_head(
    const HttpRequest& request,
    HttpEvent,
    const AuthorizationContext& auth,
    bool is_get)
{
    S3Request s3_request(request, m_domain);

    LOG_INFO("Object get or head");

    auto [status, object_get_response] = on_get_object(s3_request, auth);
    if (status->error()) {
        LOG_INFO("Failed to find object: " << status->to_string());
        return std::move(status);
    }

    LOG_INFO("Found object ok");

    auto object = object_get_response->get_item_as<HsmObject>();
    HttpResponse::Ptr response;
    LOG_INFO("Object size is: " << object->size());

    if (is_get && object->size() > 0) {
        LOG_INFO("Object has data - returning it");
        response =
            on_get_data(s3_request, request, auth, object->get_primary_key());
    }
    else {
        response = HttpResponse::create();

        if (!is_get) {
            response->header().set_item(
                "content-length", std::to_string(object->size()));
        }
    }

    Map metadata;
    m_object_adatper->get_headers(
        s3_request.get_bucket_name(), *object, metadata);
    auto on_item =
        [&response](const std::string& key, const std::string& value) {
            response->header().set_item(S3Path::meta_prefix + key, value);
        };
    metadata.for_each_item(on_item);
    return response;
}

HttpResponse::Ptr S3ObjectView::on_get_data(
    const S3Request& s3_request,
    const HttpRequest& request,
    const AuthorizationContext& auth,
    const std::string& object_id)
{
    LOG_INFO("Found object with non-zero size - sending in stream");

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    action.set_subject_key(object_id);

    const auto preferred_node =
        request.get_header().get_item("hestia-preferred_node");
    LOG_INFO("Setting preferred node to: " << preferred_node);
    action.set_preferred_node_address(preferred_node);

    std::string redirect_location;

    auto response = HttpResponse::create();
    response->set_completion_status(
        HttpResponse::CompletionStatus::AWAITING_BODY_CHUNK);

    auto completion_cb =
        [request_context = request.get_context(), &s3_request,
         &redirect_location](HsmActionResponse::Ptr response_ret) {
            if (response_ret->ok()) {
                LOG_INFO("Data action completed sucessfully");
                if (!response_ret->get_redirect_location().empty()) {
                    redirect_location = response_ret->get_redirect_location();
                    LOG_INFO("Got redirect location " << redirect_location);
                }
                else {
                    auto response = HttpResponse::create();
                    request_context->update_response(std::move(response));
                }
            }
            else {
                const std::string msg = "Error in data action \n"
                                        + response_ret->get_error().to_string();
                LOG_ERROR(msg);
                auto response = S3ViewUtils::on_server_error(s3_request, msg);
                request_context->set_response(HttpResponse::create(
                    {HttpStatus::Code::_500_INTERNAL_SERVER_ERROR, msg}));
            }
        };
    m_service->do_hsm_action(
        HsmActionRequest(
            action, {auth.m_user_id, auth.m_user_token},
            HsmNodeInterface::Type::S3),
        request.get_context()->get_stream(), completion_cb);

    // using namespace std::chrono_literals;
    // std::this_thread::sleep_for(1000ms);

    if (!redirect_location.empty()) {
        response          = HttpResponse::create(307, "Found");
        redirect_location = m_service->hostname_to_address(redirect_location);
        std::string location =
            "http://" + redirect_location + request.get_path();
        LOG_INFO("Redirecting to: " + location);
        response->header().set_item("Location", location);
    }

    if (request.get_context()->has_response()
        && request.get_context()->get_response()->error()) {
        return std::make_unique<HttpResponse>(
            *request.get_context()->get_response());
    }
    return response;
}

HttpResponse::Ptr S3ObjectView::on_copy_object(
    const S3Request& s3_request,
    const HttpRequest& request,
    const AuthorizationContext& auth,
    const std::string& copy_source)
{
    S3Path copy_path(copy_source);

    // Copy from self - i.e. update existing
    if (copy_path.is_same_resource(
            s3_request.get_bucket_name(), s3_request.get_object_key())) {
        Map attributes =
            request.get_header().get_items_with_prefix(S3Path::meta_prefix);

        CrudAttributes::FormatSpec attr_format;
        auto create_response = m_service->make_request(
            CrudRequest{
                CrudMethod::UPDATE,
                {S3ViewUtils::path_to_crud_id(s3_request.m_path),
                 CrudAttributes(attributes, attr_format),
                 CrudQuery::BodyFormat::ITEM},
                auth},
            HsmItem::hsm_object_name);
        if (!create_response->ok()) {
            const auto msg = create_response->get_error().to_string();
            LOG_ERROR(msg);
            return S3ViewUtils::on_server_error(s3_request, msg);
        }
    }
    else {
        // copy from other
        /*
            auto update_response = m_service->make_request(
                TypedCrudRequest<HsmObject>(
                    CrudMethod::UPDATE, object,
                    {auth.m_user_id, auth.m_user_token}),
                HsmItem::hsm_object_name);
            if (!update_response->ok()) {
                LOG_ERROR(update_response->get_error().to_string());
                return HttpResponse::create(500, "Internal Server Error");
            }
        */
    }
    return HttpResponse::create();
}

HttpResponse::Ptr S3ObjectView::on_put_object(
    const S3Request& s3_request,
    const HttpRequest& request,
    const AuthorizationContext& auth)
{
    auto [object_status, object_get_response] =
        on_get_object(s3_request, auth, false);
    if (object_status->error()) {
        return std::move(object_status);
    }

    std::string object_uuid;
    if (object_get_response->found()) {
        auto object = object_get_response->get_item_as<HsmObject>();
        if (object->size() > 0) {
            auto remove_response = on_delete_object(
                s3_request, auth,
                object_get_response->get_item()->get_primary_key());
            if (remove_response->error()) {
                return remove_response;
            }
        }
        else {
            object_uuid = object_get_response->get_item()->get_primary_key();
        }
    }

    if (object_uuid.empty()) {
        auto [status, get_bucket_response] = on_get_bucket(s3_request, auth);
        if (status->error()) {
            return std::move(status);
        }

        CrudIdentifier object_id;
        object_id.set_name(s3_request.get_object_key());
        object_id.set_parent_primary_key(
            get_bucket_response->get_item()->get_primary_key());

        auto create_response = m_service->make_request(
            CrudRequest{
                CrudMethod::CREATE,
                {object_id, {}, CrudQuery::BodyFormat::ITEM},
                auth},
            HsmItem::hsm_object_name);
        if (!create_response->ok()) {
            const auto msg = create_response->get_error().to_string();
            LOG_ERROR(msg);
            return S3ViewUtils::on_server_error(s3_request, msg);
        }
        object_uuid = create_response->get_item()->get_primary_key();
    }

    Map user_attributes =
        request.get_header().get_items_with_prefix(S3Path::meta_prefix);
    CrudAttributes::FormatSpec attr_format;

    if (!user_attributes.empty()) {
        CrudIdentifier metadata_id;
        metadata_id.set_parent_primary_key(object_uuid);

        auto md_get_response = m_service->make_request(
            CrudRequest{
                CrudMethod::READ,
                CrudQuery{metadata_id, CrudQuery::BodyFormat::ITEM},
                {auth.m_user_id, auth.m_user_token}},
            HsmItem::user_metadata_name);
        if (!md_get_response->ok()) {
            const auto msg = md_get_response->get_error().to_string();
            LOG_ERROR(msg);
            return S3ViewUtils::on_server_error(s3_request, msg);
        }

        auto md = *md_get_response->get_item_as<UserMetadata>();
        for (const auto& [key, value] : user_attributes.data()) {
            md.set_item(key, value);
        }

        auto md_put_response = m_service->make_request(
            TypedCrudRequest{
                CrudMethod::UPDATE, md, {auth.m_user_id, auth.m_user_token}},
            HsmItem::user_metadata_name);
        if (!md_put_response->ok()) {
            const auto msg = md_put_response->get_error().to_string();
            LOG_ERROR(msg);
            return S3ViewUtils::on_server_error(s3_request, msg);
        }
    }

    LOG_INFO("Checking whether to put data");
    if (const auto content_length =
            request.get_header().get_content_length_as_size_t();
        content_length > 0) {
        LOG_INFO("Putting data");
        return on_put_data(request, auth, object_uuid, content_length);
    }
    else {
        return HttpResponse::create();
    }
}

HttpResponse::Ptr S3ObjectView::on_put_data(
    const HttpRequest& request,
    const AuthorizationContext& auth,
    const std::string& object_id,
    std::size_t content_length)
{
    auto response = HttpResponse::create();

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    action.set_subject_key(object_id);
    action.set_size(content_length);
    std::string redirect_location;

    auto completion_cb =
        [&response, &redirect_location](HsmActionResponse::Ptr response_ret) {
            if (response_ret->ok()) {
                LOG_INFO("Data action completed sucessfully");
                if (!response_ret->get_redirect_location().empty()) {
                    redirect_location = response_ret->get_redirect_location();
                }
            }
            else {
                LOG_ERROR(
                    "Error in data action \n"
                    << response_ret->get_error().to_string());
                response = HttpResponse::create(500, "Internal Server Error.");
            }
        };
    m_service->do_hsm_action(
        HsmActionRequest(
            action, {auth.m_user_id, auth.m_user_token},
            HsmNodeInterface::Type::S3),
        request.get_context()->get_stream(), completion_cb);

    if (!redirect_location.empty()) {
        response          = HttpResponse::create(307, "Found");
        redirect_location = m_service->hostname_to_address(redirect_location);
        std::string redirect_path =
            "http://" + redirect_location + request.get_path();
        LOG_INFO("Returning redirect to: " + redirect_path);
        response->header().set_item("Location", redirect_path);
    }
    else {
        response->set_completion_status(
            HttpResponse::CompletionStatus::AWAITING_BODY_CHUNK);
    }
    return response;
}

// Handles put_object and copy_object calls
HttpResponse::Ptr S3ObjectView::on_put(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::HEADERS) {
        // This says 'just continue streaming' to the webserver
        return HttpResponse::create();
    }

    S3Request s3_request(request, m_domain);

    const auto copy_source = request.get_header().get_item("x-amz-copy-source");
    if (!copy_source.empty()) {
        return on_copy_object(s3_request, request, auth, copy_source);
    }
    else {
        return on_put_object(s3_request, request, auth);
    }
}

std::pair<HttpResponse::Ptr, CrudResponsePtr> S3ObjectView::on_get_object(
    const S3Request& s3_request,
    const AuthorizationContext& auth,
    bool error_if_not_found,
    const std::string& object_key,
    const std::string& bucket_name) const
{
    auto [status, get_bucket_response] =
        on_get_bucket(s3_request, auth, false, bucket_name);
    if (status->error()) {
        return {std::move(status), nullptr};
    }

    if (!get_bucket_response->found()) {
        return {S3ViewUtils::on_no_such_key(s3_request, true), nullptr};
    }

    CrudIdentifier object_id;
    object_id.set_name(
        object_key.empty() ? s3_request.get_object_key() : object_key);
    object_id.set_parent_primary_key(
        get_bucket_response->get_item()->get_primary_key());

    LOG_INFO(
        "Looking for object with name: " << object_id.get_name()
                                         << " and parent key "
                                         << object_id.get_parent_primary_key());

    auto obj_get_response = m_service->make_request(
        CrudRequest{
            CrudMethod::READ,
            CrudQuery{object_id, CrudQuery::BodyFormat::ITEM},
            {auth.m_user_id, auth.m_user_token}},
        HsmItem::hsm_object_name);
    if (!obj_get_response->ok()) {
        const std::string msg = obj_get_response->get_error().to_string();
        LOG_ERROR(msg);
        return {S3ViewUtils::on_server_error(s3_request, msg), nullptr};
    }
    if (!obj_get_response->found() && error_if_not_found) {
        return {S3ViewUtils::on_no_such_key(s3_request), nullptr};
    }
    return {HttpResponse::create(), std::move(obj_get_response)};
}

HttpResponse::Ptr S3ObjectView::on_delete(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    LOG_INFO("S3ObjectView:on_delete");

    S3Request s3_request(request, m_domain);

    auto [status, object_get_response] = on_get_object(s3_request, auth);
    if (status->error()) {
        return std::move(status);
    }

    auto remove_response = on_delete_object(
        s3_request, auth, object_get_response->get_item()->get_primary_key());
    if (remove_response->error()) {
        return remove_response;
    }
    else {
        return HttpResponse::create(HttpStatus::Code::_204_NO_CONTENT);
    }
}

HttpResponse::Ptr S3ObjectView::on_delete_object(
    const S3Request& s3_request,
    const AuthorizationContext& auth,
    const std::string& key) const
{
    auto response = m_service->make_request(
        CrudRequest{
            CrudMethod::REMOVE, {key}, {auth.m_user_id, auth.m_user_token}},
        HsmItem::hsm_object_name);
    if (!response->ok()) {
        const auto msg = response->get_error().to_string();
        LOG_ERROR(msg);
        return S3ViewUtils::on_server_error(s3_request, msg);
    }
    return HttpResponse::create();
}
}  // namespace hestia