#pragma once

#include "ObjectStoreRequest.h"
#include "Uuid.h"

namespace hestia {
enum class HsmObjectStoreRequestMethod {
    PUT,
    GET,
    REMOVE,
    REMOVE_ALL,
    COPY,
    MOVE,
    EXISTS
};

class HsmObjectStoreRequest :
    public MethodObjectStoreRequest<HsmObjectStoreRequestMethod> {
  public:
    HsmObjectStoreRequest(
        const std::string& object_id, HsmObjectStoreRequestMethod method);
    HsmObjectStoreRequest(
        const StorageObject& object, HsmObjectStoreRequestMethod method);
    HsmObjectStoreRequest(
        const Uuid& object_id, HsmObjectStoreRequestMethod method);
    HsmObjectStoreRequest(const ObjectStoreRequest& request);

    bool is_hsm_only_request() const;

    std::string method_as_string() const override;

    void set_source_tier(std::string tier) { m_source_tier = tier; }

    void set_target_tier(std::string tier) { m_target_tier = tier; }

    void set_action_id(const std::string& id) { m_action_id = id; }

    const std::string& get_action_id() const { return m_action_id; }

    const std::string& source_tier() const;
    const std::string& target_tier() const;

    std::string to_string() const;

    static ObjectStoreRequest to_base_request(
        const HsmObjectStoreRequest& reqeust);
    static bool is_hsm_supported_method(ObjectStoreRequestMethod method);
    static std::string to_string(HsmObjectStoreRequestMethod method);

  private:
    static bool is_copy_or_move_request(HsmObjectStoreRequestMethod method);

    static HsmObjectStoreRequestMethod from_base_method(
        ObjectStoreRequestMethod method);
    static ObjectStoreRequestMethod to_base_method(
        HsmObjectStoreRequestMethod method);

    std::string m_target_tier{0};
    std::string m_source_tier{0};
    std::string m_action_id;
};
}  // namespace hestia