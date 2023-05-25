#include "MockMotrInterfaceImpl.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include <iostream>

namespace hestia {
class MotrObject {
  public:
    MotrObject(const hestia::Uuid& oid) : m_id(oid) {}

    ~MotrObject() = default;

    mock::motr::Id get_motr_id() const { return to_motr_id(m_id); }

    mock::motr::Obj* get_motr_obj() const { return m_handle; }

    static mock::motr::Id to_motr_id(const hestia::Uuid& id)
    {
        mock::motr::Id motr_id;
        motr_id.m_lo = id.m_lo;
        motr_id.m_hi = id.m_hi;
        return motr_id;
    }

  private:
    hestia::Uuid m_id;
    mock::motr::Obj* m_handle;
};


void MockMotrInterfaceImpl::initialize(const MotrConfig& config)
{
    static constexpr int mo_uber_realm = 0;

    m_hsm.motr()->m0_client_init(&m_client, nullptr, true);
    mock::motr::Id realm_id{mo_uber_realm};
    m_hsm.motr()->m0_container_init(
        &m_container, nullptr, &realm_id, &m_client);

    std::size_t pool_count{0};
    for (const auto& entry : config.m_tier_info) {
        (void)entry;
        m_client.add_pool({pool_count});
        pool_count++;
    }

    initialize_hsm(config.m_tier_info);
}

void MockMotrInterfaceImpl::initialize_hsm(
    const std::vector<MotrHsmTierInfo>& tier_info)
{
    std::vector<hestia::Uuid> pool_fids;
    std::size_t pool_count{0};
    for (const auto& entry : tier_info) {
        (void)entry;
        pool_fids.push_back({pool_count});
        pool_count++;
    }

    mock::motr::HsmOptions hsm_options;
    hsm_options.m_pool_fids = pool_fids;

    m_hsm.m0hsm_init(&m_client, &m_container.m_co_realm, &hsm_options);
}

void MockMotrInterfaceImpl::put(
    const HsmObjectStoreRequest& request, hestia::Stream* stream) const
{
    (void)request;
    (void)stream;
    /*
    MotrObject motr_obj(request.object().id());

    auto rc = mHsm.m0hsm_create(motr_obj.getMotrId(), motr_obj.getMotrObj(),
    request.targetTier(), false); if (rc < 0)
    {
        std::cerr << "hsm obj create failed." << std::endl;
        return rc;
    }

    rc = mHsm.m0hsm_set_write_tier(motr_obj.getMotrId(), request.targetTier());
    if (rc < 0)
    {
        std::cerr << "Failed to set write tier." << std::endl;
        return rc;
    }

    auto sink = hestia::InMemoryStreamSink::Create();

    rc = mHsm.m0hsm_pwrite(motr_obj.getMotrObj(), buffer->asVoid(),
    buffer->length(), request.extent().mOffset);

    */
}

void MockMotrInterfaceImpl::get(
    const HsmObjectStoreRequest& request,
    hestia::StorageObject& object,
    hestia::Stream* stream) const
{
    (void)request;
    (void)object;
    (void)stream;
    /*
    MotrObject motr_obj(request.object().id());

    auto rc = mHsm.m0hsm_set_read_tier(motr_obj.getMotrId(),
    request.sourceTier()); if (rc < 0)
    {
        std::cerr << "Failed to set read tier." << std::endl;
        return rc;
    }

    auto source = hestia::InMemoryStreamSource::Create();

    rc = mHsm.m0hsm_read(motr_obj.getMotrId(),
    const_cast<void*>(buffer->asVoid()), buffer->length(),
    request.extent().mOffset);
    */
}

void MockMotrInterfaceImpl::remove(const HsmObjectStoreRequest& request) const
{
    (void)request;
    /*
    MotrObject motr_obj(request.object().id());

    std::size_t offset{0};
    std::size_t length{IMotrInterfaceImpl::MAX_OBJ_LENGTH};

    mock::motr::hsm_rls_flags flags =
    mock::motr::hsm_rls_flags::HSM_KEEP_LATEST; auto rc =
    mHsm.m0hsm_release(motr_obj.getMotrId(), request.sourceTier(), offset,
    length, flags);

    */
}

void MockMotrInterfaceImpl::copy(const HsmObjectStoreRequest& request) const
{
    (void)request;
    /*
    MotrObject motr_obj(request.object().id());
    // std::size_t length{IMotrInterfaceImpl::MAX_OBJ_LENGTH};

    mock::motr::Hsm::hsm_cp_flags flags =
    mock::motr::Hsm::hsm_cp_flags::HSM_KEEP_OLD_VERS; auto rc =
    mHsm.m0hsm_copy(motr_obj.getMotrId(), request.sourceTier(),
    request.targetTier(), request.extent().mOffset, request.extent().mLength,
    flags);
    */
}

void MockMotrInterfaceImpl::move(const HsmObjectStoreRequest& request) const
{
    (void)request;
    /*
    MotrObject motr_obj(request.object().id());

    //std::size_t length{IMotrInterfaceImpl::MAX_OBJ_LENGTH};

    mock::motr::Hsm::hsm_cp_flags flags =
    mock::motr::Hsm::hsm_cp_flags::HSM_MOVE; auto rc =
    mHsm.m0hsm_copy(motr_obj.getMotrId(), request.sourceTier(),
    request.targetTier(), request.extent().mOffset, request.extent().mLength,
    flags);
    */
}
}  // namespace hestia