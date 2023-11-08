#include "MockMotrInterfaceImpl.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "Logger.h"
#include "UuidUtils.h"

#include <stdexcept>

namespace hestia {
class MotrObject {
  public:
    MotrObject(const std::string& oid) : m_id(oid) {}

    ~MotrObject() = default;

    mock::motr::Id get_motr_id() const { return to_motr_id(m_id); }

    mock::motr::Obj* get_motr_obj() { return &m_handle; }

    static mock::motr::Id to_motr_id(const std::string& id)
    {
        mock::motr::Id motr_id = UuidUtils::from_string(id);
        return motr_id;
    }

    std::size_t m_size{0};

  private:
    std::string m_id;
    mock::motr::Obj m_handle;
};


void MockMotrInterfaceImpl::initialize(const MotrConfig& config)
{
    static constexpr int mo_uber_realm = 0;

    m_hsm.motr()->m0_client_init(&m_client, nullptr, true);
    mock::motr::Id realm_id{mo_uber_realm, 0};
    m_hsm.motr()->m0_container_init(
        &m_container, nullptr, &realm_id, &m_client);

    std::size_t pool_count{0};
    LOG_INFO(
        "Initializing with: " << config.m_tier_info.container().size()
                              << " pools.");
    for (const auto& entry : config.m_tier_info.container()) {
        (void)entry;
        m_client.add_pool({pool_count, 0});
        pool_count++;
    }

    initialize_hsm(config.m_tier_info.container());
}

void MockMotrInterfaceImpl::initialize_hsm(
    const std::vector<MotrHsmTierInfo>& tier_info)
{
    std::vector<hestia::Uuid> pool_fids;
    std::size_t pool_count{0};
    for (const auto& entry : tier_info) {
        (void)entry;
        pool_fids.push_back({pool_count, 0});
        pool_count++;
    }

    mock::motr::HsmOptions hsm_options;
    hsm_options.m_pool_fids = pool_fids;

    m_hsm.m0hsm_init(&m_client, &m_container.m_co_realm, &hsm_options);
}

void MockMotrInterfaceImpl::put(
    const HsmObjectStoreRequest& request,
    Stream* stream,
    completionFunc completion_func,
    progressFunc) const
{
    MotrObject motr_obj(request.object().id());

    auto rc = m_hsm.m0hsm_create(
        motr_obj.get_motr_id(), motr_obj.get_motr_obj(), request.target_tier(),
        false);
    if (rc < 0) {
        const std::string msg =
            "Failed to create object: " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }

    rc = m_hsm.m0hsm_set_write_tier(
        motr_obj.get_motr_id(), request.target_tier());
    if (rc < 0) {
        const std::string msg =
            "Failed to set write tier: " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }

    auto sink_func = [this, motr_obj](
                         const ReadableBufferView& buffer,
                         std::size_t offset) -> InMemoryStreamSink::Status {
        LOG_INFO(
            "Writing buffer with size: " << buffer.length() << " and offset "
                                         << offset);

        auto working_obj = motr_obj;
        auto rc          = m_hsm.m0hsm_pwrite(
            working_obj.get_motr_obj(), const_cast<void*>(buffer.as_void()),
            buffer.length(), offset);
        if (rc < 0) {
            std::string msg =
                "Error writing buffer at offset " + std::to_string(offset);
            LOG_ERROR(msg);
            return {false, 0};
        }
        return {true, buffer.length()};
    };

    auto sink = InMemoryStreamSink::create(sink_func);
    stream->set_sink(std::move(sink));

    auto stream_completion_cb = [request, completion_func](StreamState state) {
        auto response = HsmObjectStoreResponse::create(request, "");
        if (!state.ok()) {
            response->on_error(
                {HsmObjectStoreErrorCode::ERROR, state.message()});
        }
        completion_func(std::move(response));
    };
    stream->set_completion_func(stream_completion_cb);
}

void MockMotrInterfaceImpl::get(
    const HsmObjectStoreRequest& request,
    Stream* stream,
    completionFunc completion_func,
    progressFunc) const
{
    MotrObject motr_obj(request.object().id());
    motr_obj.m_size = request.object().size();

    auto rc = m_hsm.m0hsm_set_read_tier(
        motr_obj.get_motr_id(), request.source_tier());
    if (rc < 0) {
        const std::string msg =
            "Failed to set read tier: " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }

    auto source_func = [this, motr_obj](
                           WriteableBufferView& buffer,
                           std::size_t offset) -> InMemoryStreamSource::Status {
        if (offset >= motr_obj.m_size) {
            return {true, 0};
        }

        auto read_size = buffer.length();
        if (offset + buffer.length() > motr_obj.m_size) {
            read_size = motr_obj.m_size - offset;
        }

        auto rc = m_hsm.m0hsm_read(
            motr_obj.get_motr_id(), buffer.as_void(), read_size, offset);
        if (rc < 0) {
            std::string msg =
                "Error writing buffer at offset " + std::to_string(offset);
            LOG_ERROR(msg);
            return {false, 0};
        }
        return {true, read_size};
    };

    LOG_INFO(
        "Setting source func for object: "
        << request.object().id() << " with size " << request.object().size());

    auto source = hestia::InMemoryStreamSource::create(source_func);
    stream->set_source(std::move(source));

    auto stream_completion_cb = [request, completion_func](StreamState state) {
        auto response = HsmObjectStoreResponse::create(request, "");
        if (!state.ok()) {
            response->on_error(
                {HsmObjectStoreErrorCode::ERROR, state.message()});
        }
        completion_func(std::move(response));
    };
    stream->set_completion_func(stream_completion_cb);
}

void MockMotrInterfaceImpl::remove(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc) const
{
    MotrObject motr_obj(request.object().id());

    std::size_t offset{0};
    std::size_t length{IMotrInterfaceImpl::max_obj_length};
    mock::motr::hsm_rls_flags flags =
        mock::motr::hsm_rls_flags::HSM_KEEP_LATEST;
    auto rc = m_hsm.m0hsm_release(
        motr_obj.get_motr_id(), request.source_tier(), offset, length, flags);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_release" + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
    completion_func(HsmObjectStoreResponse::create(request, ""));
}

void MockMotrInterfaceImpl::copy(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc) const
{
    MotrObject motr_obj(request.object().id());
    std::size_t length = request.extent().m_length;
    if (length == 0) {
        length = IMotrInterfaceImpl::max_obj_length;
    }

    mock::motr::Hsm::hsm_cp_flags flags =
        mock::motr::Hsm::hsm_cp_flags::HSM_KEEP_OLD_VERS;
    auto rc = m_hsm.m0hsm_copy(
        motr_obj.get_motr_id(), request.source_tier(), request.target_tier(),
        request.extent().m_offset, length, flags);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_copy" + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
    completion_func(HsmObjectStoreResponse::create(request, ""));
}

void MockMotrInterfaceImpl::move(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc) const
{
    MotrObject motr_obj(request.object().id());
    std::size_t length = request.extent().m_length;
    if (length == 0) {
        length = IMotrInterfaceImpl::max_obj_length;
    }

    mock::motr::Hsm::hsm_cp_flags flags =
        mock::motr::Hsm::hsm_cp_flags::HSM_MOVE;
    auto rc = m_hsm.m0hsm_copy(
        motr_obj.get_motr_id(), request.source_tier(), request.target_tier(),
        request.extent().m_offset, length, flags);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_copy - move " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
    completion_func(HsmObjectStoreResponse::create(request, ""));
}
}  // namespace hestia