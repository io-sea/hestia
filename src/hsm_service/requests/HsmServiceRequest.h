#pragma once

#include <ostk/Extent.h>
#include <ostk/Request.h>
#include <ostk/StorageObject.h>
#include <ostk/Uuid.h>

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
    public ostk::BaseRequest,
    public ostk::MethodRequest<HsmServiceRequestMethod> {
  public:
    HsmServiceRequest(
        const ostk::Uuid& object_id, HsmServiceRequestMethod method);

    HsmServiceRequest(
        const ostk::StorageObject& object, HsmServiceRequestMethod method);

    HsmServiceRequest(HsmServiceRequestMethod method);

    const ostk::Extent& extent() const { return m_extent; }

    const ostk::StorageObject& object() const;

    const std::string& query() const { return m_query; }

    std::string methodAsString() const override { return {}; }

    void set_extent(const ostk::Extent& extent);

    void set_target_tier(uint8_t tier);

    void set_source_tier(uint8_t tier);

    void set_should_put_overwrite(bool overwrite);

    bool should_overwrite_put() const;

    void set_query(const std::string& query);

    uint8_t source_tier() const { return m_source_tier; }

    uint8_t target_tier() const { return m_target_tier; }

  private:
    uint8_t m_target_tier{0};
    uint8_t m_source_tier{0};
    ostk::Extent m_extent;
    ostk::StorageObject m_object;

    std::string m_query;
    bool m_overwrite_if_existing{false};
};