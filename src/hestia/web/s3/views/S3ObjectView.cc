#include "S3ObjectView.h"

#include "DistributedHsmService.h"
#include "S3ObjectAdapter.h"
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
    m_service(service), m_object_adatper(std::make_unique<S3ObjectAdapter>())
{
    LOG_INFO("Loaded object view");
}

HttpResponse::Ptr S3ObjectView::on_get_or_head(
    const HttpRequest& request,
    HttpEvent,
    const AuthorizationContext& auth,
    ObjectContext& object_context)
{
    const auto s3_path = S3Path(request.get_path());

    CrudIdentifier container_id;
    container_id.set_name(s3_path.m_bucket_name);
    container_id.set_parent_primary_key(auth.m_user_id);

    CrudQuery container_query(container_id, CrudQuery::OutputFormat::ITEM);
    const auto container_get_response = m_service->make_request(
        CrudRequest{container_query, {auth.m_user_id, auth.m_user_token}},
        HsmItem::dataset_name);
    if (!container_get_response->ok()) {
        LOG_ERROR(container_get_response->get_error().to_string());
        return HttpResponse::create(
            {HttpError::Code::_500_INTERNAL_SERVER_ERROR,
             "Server error checking bucket."});
    }

    if (!container_get_response->found()) {
        return HttpResponse::create(
            {HttpError::Code::_404_NOT_FOUND, "Container not found."});
    }

    CrudIdentifier object_id;
    object_id.set_name(s3_path.m_object_key);
    object_id.set_parent_primary_key(
        container_get_response->get_item()->get_primary_key());

    LOG_INFO(
        "Looking for object with key: "
        << s3_path.m_object_key << " and container id: "
        << container_get_response->get_item()->get_primary_key());

    auto obj_get_response = m_service->make_request(
        CrudRequest{
            CrudQuery{object_id, CrudQuery::OutputFormat::ITEM},
            {auth.m_user_id, auth.m_user_token}},
        HsmItem::hsm_object_name);
    if (!obj_get_response->ok()) {
        LOG_ERROR(obj_get_response->get_error().to_string());
        return HttpResponse::create(
            {HttpError::Code::_500_INTERNAL_SERVER_ERROR,
             "Server error checking object."});
    }
    if (!obj_get_response->found()) {
        LOG_INFO("Object not found");
        return HttpResponse::create(
            {HttpError::Code::_404_NOT_FOUND, "Object not found."});
    }

    auto object = obj_get_response->get_item_as<HsmObject>();

    auto response = HttpResponse::create();
    Map metadata;
    auto dataset = container_get_response->get_item_as<Dataset>();
    m_object_adatper->get_headers(*dataset, *object, metadata);
    auto on_item =
        [&response](const std::string& key, const std::string& value) {
            response->header().set_item(S3Path::meta_prefix + key, value);
        };
    metadata.for_each_item(on_item);

    object_context.m_size = object->size();
    object_context.m_id   = object->get_primary_key();
    return response;
}

HttpResponse::Ptr S3ObjectView::on_get(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::HEADERS) {
        return HttpResponse::create();
    }

    ObjectContext object_context;
    auto response = on_get_or_head(request, event, auth, object_context);
    if (response->error()) {
        return response;
    }

    if (object_context.m_size > 0) {
        LOG_INFO("Found object with non-zero size - sending in stream");

        HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
        action.set_subject_key(object_context.m_id);
        std::string redirect_location;

        auto completion_cb = [&response, &redirect_location](
                                 HsmActionResponse::Ptr response_ret) {
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
    }
    return response;
}

HttpResponse::Ptr S3ObjectView::on_head(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    ObjectContext object_context;
    return on_get_or_head(request, event, auth, object_context);
}

HttpResponse::Ptr S3ObjectView::on_put(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (event != HttpEvent::HEADERS) {
        return HttpResponse::create();
    }

    const auto s3_path = S3Path(request.get_path());

    CrudIdentifier container_id;
    container_id.set_name(s3_path.m_bucket_name);
    container_id.set_parent_primary_key(auth.m_user_id);

    CrudQuery container_query(container_id, CrudQuery::OutputFormat::ITEM);
    const auto container_get_response = m_service->make_request(
        CrudRequest{container_query, {auth.m_user_id, auth.m_user_token}},
        HsmItem::dataset_name);
    if (!container_get_response->ok()) {
        LOG_ERROR(container_get_response->get_error().to_string());
        return HttpResponse::create(
            {HttpError::Code::_500_INTERNAL_SERVER_ERROR,
             "Server error checking bucket."});
    }

    HsmObject object;
    std::string container_key;
    if (container_get_response->found()) {
        LOG_INFO("Found container - using it for object");

        container_key = container_get_response->get_item()->get_primary_key();

        CrudIdentifier object_id;
        object_id.set_name(s3_path.m_object_key);
        object_id.set_parent_primary_key(container_key);

        auto obj_get_response = m_service->make_request(
            CrudRequest{
                CrudQuery{object_id, CrudQuery::OutputFormat::ITEM},
                {auth.m_user_id, auth.m_user_token}},
            HsmItem::hsm_object_name);
        if (!obj_get_response->ok()) {
            LOG_ERROR(obj_get_response->get_error().to_string());
            return HttpResponse::create(
                {HttpError::Code::_500_INTERNAL_SERVER_ERROR,
                 "Server error checking object."});
        }
        if (obj_get_response->found()) {
            object = *obj_get_response->get_item_as<HsmObject>();
        }
    }
    else {
        // Create container
        Dataset dataset;
        dataset.set_name(s3_path.m_bucket_name);

        auto container_create_response = m_service->make_request(
            TypedCrudRequest<Dataset>{
                CrudMethod::CREATE,
                dataset,
                {auth.m_user_id, auth.m_user_token},
                CrudQuery::OutputFormat::ID},
            HsmItem::dataset_name);
        if (!container_create_response->ok()) {
            LOG_ERROR(container_create_response->get_error().to_string());
            return HttpResponse::create(
                {HttpError::Code::_500_INTERNAL_SERVER_ERROR,
                 "Server error creating container for object."});
        }
        container_key = container_create_response->ids()[0];
    }

    if (!object.get_primary_key().empty()) {
        auto update_response = m_service->make_request(
            TypedCrudRequest<HsmObject>(
                CrudMethod::UPDATE, object,
                {auth.m_user_id, auth.m_user_token}),
            HsmItem::hsm_object_name);
        if (!update_response->ok()) {
            LOG_ERROR(update_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error");
        }
    }
    else {
        CrudIdentifier object_id;
        object_id.set_name(s3_path.m_object_key);
        object_id.set_parent_primary_key(container_key);

        LOG_INFO("Creating object with parent primary key: " << container_key);

        auto create_response = m_service->make_request(
            CrudRequest{
                CrudMethod::CREATE,
                {auth.m_user_id, auth.m_user_token},
                {object_id},
                {},
                CrudQuery::OutputFormat::ITEM},
            HsmItem::hsm_object_name);
        if (!create_response->ok()) {
            LOG_ERROR(create_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error");
        }
        object = *create_response->get_item_as<HsmObject>();
    }

    // object.m_user_metadata =
    //     request.get_header().get_items_with_prefix(S3Path::meta_prefix);

    auto response = HttpResponse::create();
    const auto content_length =
        request.get_header().get_content_length_as_size_t();

    if (content_length > 0) {
        HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
        action.set_subject_key(object.get_primary_key());
        action.set_size(content_length);
        std::string redirect_location;

        auto completion_cb = [&response, &redirect_location](
                                 HsmActionResponse::Ptr response_ret) {
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
    }
    else {
        response = HttpResponse::create(201, "Created");
    }
    return response;
}

HttpResponse::Ptr S3ObjectView::on_delete(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
{
    (void)request;
    /*
    const auto s3_path = S3Path(request.get_path());

    auto hsm_service = m_service->get_hsm_service();

    Dataset container;
    container.set_name(s3_path.m_container_name);

    auto container_get_response =
        hsm_service->make_request({container, HsmServiceRequestMethod::GET});
    if (!container_get_response->ok()) {
        LOG_ERROR(container_get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    HsmObject hsm_object;
    hsm_object.set_name(s3_path.m_object_id);

    auto obj_get_reponse =
        hsm_service->make_request({hsm_object, HsmServiceRequestMethod::GET});
    if (!obj_get_reponse->ok()) {
        LOG_ERROR(obj_get_reponse->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (!obj_get_reponse->object_found()) {
        return HttpResponse::create(404, "Not Found");
    }

    auto remove_response = hsm_service->make_request(
        {hsm_object, HsmServiceRequestMethod::RELEASE});
    if (!remove_response->ok()) {
        LOG_ERROR(remove_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    */
    return HttpResponse::create();
}
}  // namespace hestia