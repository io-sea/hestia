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

    void set_source_tier(uint8_t tier) { m_source_tier = tier; }

    void set_target_tier(uint8_t tier) { m_target_tier = tier; }

    uint8_t source_tier() const;
    uint8_t target_tier() const;

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

    uint8_t m_target_tier{0};
    uint8_t m_source_tier{0};
};
}  // namespace hestia