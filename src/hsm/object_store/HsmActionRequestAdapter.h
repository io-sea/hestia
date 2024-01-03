#pragma once

#include "HsmAction.h"
#include "HsmObjectStoreRequest.h"
#include "HttpRequest.h"

namespace hestia {
class HsmActionRequestAdapter {
  public:
    static HsmAction::Action get_action_type(
        const HsmObjectStoreRequest& request);

    static std::string get_endpoint();

    static HttpRequest::Method get_http_method(
        const HsmObjectStoreRequest& request);

    static std::string get_user_token(const HsmObjectStoreRequest& request);

    static void to_request(
        HttpRequest& req,
        const HsmAction& action,
        const std::string& user_token);

    static void to_action(
        HsmAction& action, const HsmObjectStoreRequest& request);
};
}  // namespace hestia