#include "HsmActionRequestAdapter.h"

#include "HsmItem.h"

namespace hestia {
void HsmActionRequestAdapter::to_request(
    HttpRequest& req, const HsmAction& action, const std::string& user_token)
{
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    req.get_header().set_items(flat_dict);
    if (!user_token.empty()) {
        req.get_header().set_item("Authorization", user_token);
    }
}

void HsmActionRequestAdapter::to_action(
    HsmAction& action, const HsmObjectStoreRequest& request)
{
    action.set_subject_key(request.object().get_primary_key());
    action.set_source_tier_id(request.source_tier());
    action.set_target_tier_id(request.target_tier());
    action.set_primary_key(request.get_action_id());
    action.set_size(request.extent().m_length);
    action.set_offset(request.extent().m_offset);
}

HttpRequest::Method HsmActionRequestAdapter::get_http_method(
    const HsmObjectStoreRequest& request)
{
    if (request.method() == HsmObjectStoreRequestMethod::GET) {
        return HttpRequest::Method::GET;
    }
    else {
        return HttpRequest::Method::PUT;
    }
}

std::string HsmActionRequestAdapter::get_user_token(
    const HsmObjectStoreRequest& request)
{
    return request.object().metadata().get_item("hestia-user_token");
}

std::string HsmActionRequestAdapter::get_endpoint()
{
    return "/api/v1/" + std::string(HsmItem::hsm_action_name) + "s";
}

HsmAction::Action HsmActionRequestAdapter::get_action_type(
    const HsmObjectStoreRequest& request)
{
    switch (request.method()) {
        case HsmObjectStoreRequestMethod::GET:
            return HsmAction::Action::GET_DATA;
        case HsmObjectStoreRequestMethod::PUT:
            return HsmAction::Action::PUT_DATA;
        case HsmObjectStoreRequestMethod::REMOVE:
            return HsmAction::Action::RELEASE_DATA;
        case HsmObjectStoreRequestMethod::COPY:
            return HsmAction::Action::COPY_DATA;
        case HsmObjectStoreRequestMethod::MOVE:
            return HsmAction::Action::MOVE_DATA;
        case HsmObjectStoreRequestMethod::EXISTS:
        case HsmObjectStoreRequestMethod::REMOVE_ALL:
        default:
            return HsmAction::Action::NONE;
    }
}

}  // namespace hestia