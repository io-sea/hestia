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

#include <sstream>

namespace hestia {
S3ObjectView::S3ObjectView(DistributedHsmService* service) :
    S3WebView(service), m_object_adatper(std::make_unique<S3HsmObjectAdapter>())
{
    LOG_INFO("Loaded object view");
}

HttpResponse::Ptr S3ObjectView::on_get(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::HEADERS) {
        return HttpResponse::create();
    }
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
    S3Request s3_request(request);

    LOG_INFO("Object get or head req: " << s3_request.to_string());

    auto [status, object_get_response] = on_get_object(s3_request, auth);
    if (status->error()) {
        LOG_INFO("Faile to find object: " << status->to_string());
        return std::move(status);
    }

    auto object = object_get_response->get_item_as<HsmObject>();
    HttpResponse::Ptr response;
    if (is_get && object->size() > 0) {
        response =
            on_get_data(s3_request, request, auth, object->get_primary_key());
    }
    else {
        response = HttpResponse::create();
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
    std::string redirect_location;

    auto response = HttpResponse::create();

    auto completion_cb = [&s3_request, &response, &redirect_location](
                             HsmActionResponse::Ptr response_ret) {
        if (response_ret->ok()) {
            LOG_INFO("Data action completed sucessfully");
            if (!response_ret->get_redirect_location().empty()) {
                redirect_location = response_ret->get_redirect_location();
            }
        }
        else {
            const std::string msg = "Error in data action \n"
                                    + response_ret->get_error().to_string();
            LOG_ERROR(msg);
            response = S3ViewUtils::on_server_error(s3_request, msg);
        }
    };
    m_service->do_data_io_action(
        HsmActionRequest(action, {auth.m_user_id, auth.m_user_token}),
        request.get_context()->get_stream(), completion_cb);

    if (!redirect_location.empty()) {
        response = HttpResponse::create(307, "Found");
        response->header().set_item(
            "Location", "http://" + redirect_location + m_path);
    }
    else {
        response->set_completion_status(
            HttpResponse::CompletionStatus::AWAITING_BODY_CHUNK);
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
        auto create_response = m_service->make_request(
            CrudRequest{
                CrudMethod::UPDATE,
                {auth.m_user_id, auth.m_user_token},
                {S3ViewUtils::path_to_crud_id(s3_request.m_path)},
                {attributes},
                CrudQuery::OutputFormat::ITEM},
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

    if (object_get_response->found()) {
        auto remove_response = on_delete_object(
            s3_request, auth,
            object_get_response->get_item()->get_primary_key());
        if (remove_response->error()) {
            return remove_response;
        }
    }

    auto [status, get_bucket_response] = on_get_bucket(s3_request, auth);
    if (status->error()) {
        return std::move(status);
    }

    CrudIdentifier object_id;
    object_id.set_name(s3_request.get_object_key());
    object_id.set_parent_primary_key(
        get_bucket_response->get_item()->get_primary_key());

    Map attributes =
        request.get_header().get_items_with_prefix(S3Path::meta_prefix);
    auto create_response = m_service->make_request(
        CrudRequest{
            CrudMethod::CREATE,
            {auth.m_user_id, auth.m_user_token},
            {object_id},
            {attributes},
            CrudQuery::OutputFormat::ITEM},
        HsmItem::hsm_object_name);
    if (!create_response->ok()) {
        const auto msg = create_response->get_error().to_string();
        LOG_ERROR(msg);
        return S3ViewUtils::on_server_error(s3_request, msg);
    }

    if (const auto content_length =
            request.get_header().get_content_length_as_size_t();
        content_length > 0) {
        return on_put_data(
            request, auth, create_response->get_item()->get_primary_key(),
            content_length);
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
    m_service->do_data_io_action(
        HsmActionRequest(action, {auth.m_user_id, auth.m_user_token}),
        request.get_context()->get_stream(), completion_cb);

    if (!redirect_location.empty()) {
        response = HttpResponse::create(307, "Found");
        response->header().set_item(
            "Location", "http://" + redirect_location + m_path);
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

    S3Request s3_request(request);

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
            CrudQuery{object_id, CrudQuery::OutputFormat::ITEM},
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

    S3Request s3_request(request);

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
            CrudMethod::REMOVE, {auth.m_user_id, auth.m_user_token}, {key}},
        HsmItem::hsm_object_name);
    if (!response->ok()) {
        const auto msg = response->get_error().to_string();
        LOG_ERROR(msg);
        return S3ViewUtils::on_server_error(s3_request, msg);
    }
    return HttpResponse::create();
}
}  // namespace hestia