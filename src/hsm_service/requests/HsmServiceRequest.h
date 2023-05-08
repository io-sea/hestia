#pragma once

#include "Extent.h"
#include "Request.h"
#include "StorageObject.h"
#include "Uuid.h"

#include <memory>

namespace hestia {
enum class HsmServiceRequestMethod {
    PUT,
    GET,
    GET_TIERS,
    REMOVE,
    REMOVE_ALL,
    COPY,
    MOVE,
    LIST,
    LIST_TIERS
};

class HsmServiceRequest :
    public hestia::BaseRequest,
    public hestia::MethodRequest<HsmServiceRequestMethod> {
  public:
    using Ptr = std::unique_ptr<HsmServiceRequest>;

    HsmServiceRequest(
        const hestia::Uuid& object_id, HsmServiceRequestMethod method);

    HsmServiceRequest(
        const hestia::StorageObject& object, HsmServiceRequestMethod method);

    HsmServiceRequest(HsmServiceRequestMethod method);

    const hestia::Extent& extent() const { return m_extent; }

    const hestia::StorageObject& object() const;

    const std::string& query() const { return m_query; }

    std::string method_as_string() const override;

    void set_extent(const hestia::Extent& extent);

    void set_target_tier(uint8_t tier);

    void set_source_tier(uint8_t tier);

    void set_should_put_overwrite(bool overwrite);

    bool should_overwrite_put() const;

    void set_query(const std::string& query);

    uint8_t source_tier() const { return m_source_tier; }

    uint8_t target_tier() const { return m_target_tier; }

    std::string to_string() const;

  private:
    uint8_t m_target_tier{0};
    uint8_t m_source_tier{0};
    hestia::Extent m_extent;
    hestia::StorageObject m_object;

    std::string m_query;
    bool m_overwrite_if_existing{false};
};
}  // namespace hestia