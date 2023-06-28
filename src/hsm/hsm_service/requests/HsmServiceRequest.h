#pragma once

#include "Dataset.h"
#include "Extent.h"
#include "HsmObject.h"
#include "Request.h"
#include "Uuid.h"

#include <memory>

namespace hestia {
enum class HsmServiceRequestMethod {
    CREATE,
    PUT,
    GET,
    EXISTS,
    GET_TIERS,
    REMOVE,
    REMOVE_ALL,
    COPY,
    MOVE,
    LIST,
    LIST_TIERS,
    LIST_ATTRIBUTES
};

enum class HsmServiceRequestSubject { OBJECT, DATASET };

class HsmServiceRequest :
    public BaseRequest,
    public MethodRequest<HsmServiceRequestMethod> {
  public:
    using Ptr = std::unique_ptr<HsmServiceRequest>;

    HsmServiceRequest(
        const Uuid& subject_id,
        HsmServiceRequestSubject subject,
        HsmServiceRequestMethod method);

    HsmServiceRequest(const HsmObject& object, HsmServiceRequestMethod method);

    HsmServiceRequest(const Dataset& dataset, HsmServiceRequestMethod method);

    HsmServiceRequest(
        const uint8_t& tier_id,
        HsmServiceRequestSubject subject,
        HsmServiceRequestMethod method);

    HsmServiceRequest(
        HsmServiceRequestSubject subject, HsmServiceRequestMethod method);

    const Extent& extent() const;

    const HsmObject& object() const;

    HsmObject& object();

    const uint8_t& tier() const;

    uint8_t& tier();

    const std::string& query() const;

    std::string method_as_string() const override;

    void set_extent(const Extent& extent);

    void set_target_tier(uint8_t tier);

    void set_source_tier(uint8_t tier);

    void set_should_put_overwrite(bool overwrite);

    bool should_overwrite_put() const;

    void set_query(const std::string& query);

    uint8_t source_tier() const;

    uint8_t target_tier() const;

    std::string to_string() const;

    HsmServiceRequestSubject get_subject() const { return m_subject; }

  private:
    uint8_t m_target_tier{0};
    uint8_t m_source_tier{0};
    Extent m_extent;
    HsmObject m_object;
    Dataset m_dataset;

    HsmServiceRequestSubject m_subject{HsmServiceRequestSubject::OBJECT};
    uint8_t m_tier;

    std::string m_query;
    bool m_overwrite_if_existing{false};
};
}  // namespace hestia
