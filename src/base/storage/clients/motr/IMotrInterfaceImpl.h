#pragma once

#include "HsmObjectStoreResponse.h"
#include "MotrConfig.h"

#include "Stream.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace hestia {
class IMotrInterfaceImpl {
  public:
    using completionFunc = std::function<void(HsmObjectStoreResponse::Ptr)>;
    using progressFunc   = std::function<void(HsmObjectStoreResponse::Ptr)>;

    virtual ~IMotrInterfaceImpl()                     = default;
    virtual void initialize(const MotrConfig& config) = 0;

    virtual void copy(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    virtual void get(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    virtual void move(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    virtual void put(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    virtual void remove(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    void set_tier_ids(const std::vector<std::string>& ids) { m_tier_ids = ids; }

    std::size_t get_tier_index(const std::string& id) const
    {
        auto it = std::find(m_tier_ids.begin(), m_tier_ids.end(), id);
        if (it == m_tier_ids.end()) {
            throw std::runtime_error(
                "Requested tier id " + id + "not found in object store cache.");
        }
        return 1 + std::distance(m_tier_ids.begin(), it);
    }

    static constexpr std::size_t max_obj_length{0xffffffffffffffff};

  protected:
    std::vector<std::string> m_tier_ids;
    virtual void initialize_hsm(const std::vector<MotrHsmTierInfo>&) = 0;
};
}  // namespace hestia