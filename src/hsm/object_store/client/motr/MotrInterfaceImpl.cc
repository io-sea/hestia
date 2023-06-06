#include "MotrInterfaceImpl.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "Logger.h"

#include "File.h"

#include <iostream>

#ifdef HAS_MOTR
#include "motr/idx.h"

namespace hestia {
void MotrInterfaceImpl::initialize(const MotrConfig& config)
{
    LOG_INFO("Initializing motr client with config: " << config.to_string());

    m_config = config;

    m_m0tr_config.mc_is_oostore     = true;
    m_m0tr_config.mc_is_read_verify = false;

    m_m0tr_config.mc_local_addr  = m_config.m_local_address.c_str();
    m_m0tr_config.mc_ha_addr     = m_config.m_ha_address.c_str();
    m_m0tr_config.mc_profile     = m_config.m_profile.c_str();
    m_m0tr_config.mc_process_fid = m_config.m_proc_fid.c_str();

    m_m0tr_config.mc_tm_recv_queue_min_len = 64;
    m_m0tr_config.mc_max_rpc_msg_size      = 65536;

    m_m0tr_config.mc_layout_id = 0;

    m_m0tr_config.mc_idx_service_id = M0_IDX_DIX;
    struct m0_idx_dix_config dix_conf;
    dix_conf.kc_create_meta           = false;
    m_m0tr_config.mc_idx_service_conf = &dix_conf;

    const auto rc = m0_client_init(&m_client_instance, &m_m0tr_config, true);
    if (rc < 0) {
        LOG_ERROR("m0 client init failed: " << rc);
    }

    initialize_hsm(config.m_tier_info);
}

void MotrInterfaceImpl::finish()
{
    if (m_client_instance) {
        m0_client_fini(m_client_instance, false);
        m_client_instance = nullptr;
    }
}

void MotrInterfaceImpl::initialize_hsm(
    const std::vector<MotrHsmTierInfo>& tier_info)
{
    const auto tier_info_str = MotrConfig::get_tier_info_for_m0hsm(tier_info);

    LOG_INFO("Initializeing m0hsm");
    LOG_INFO("Writing hsm config: " << tier_info_str);

    const auto path = std::filesystem::current_path() / "hsm.config";
    auto out_stream = std::ofstream(path);
    out_stream << tier_info_str;
    out_stream.close();

    m0hsm_options hsm_options;
    hsm_options.trace_level = LOG_INFO;
    hsm_options.op_timeout  = 10;
    hsm_options.log_stream  = stderr;

    FILE* f_in         = ::fopen(path.string().c_str(), "r");
    hsm_options.rcfile = f_in;

    m0_container_init(&m_container, nullptr, &M0_UBER_REALM, m_client_instance);
    auto rc = m_container.co_realm.re_entity.en_sm.sm_rc;
    if (rc != 0) {
        LOG_ERROR("m0_container_init init failed: " << rc);
        return;
    }
    m_realm = m_container.co_realm;

    rc = m0hsm_init(m_client_instance, &m_realm, &hsm_options);
    if (rc < 0) {
        LOG_ERROR("m0hsm_init init failed: " << rc);
    }

    ::fclose(f_in);
}

void MotrInterfaceImpl::put(
    const HsmObjectStoreRequest& request, hestia::Stream* stream) const
{
    LOG_INFO("Starting m0hsm put");
    Uuid uuid;
    uuid.from_string(request.object().id());

    struct m0_uint128 id = M0_ID_APP;
    id.u_hi              = 0;
    id.u_lo += uuid.m_lo;

    struct m0_obj motr_obj;
    memset(&motr_obj, 0, sizeof(m0_obj));

    LOG_INFO("Creating object with id: [" << id.u_hi << ", " << id.u_lo << "]");
    auto rc = m0hsm_create(id, &motr_obj, request.target_tier(), false);
    if (rc < 0) {
        const std::string msg =
            "Failed to create object: " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }

    LOG_INFO("Setting write tier: " << request.target_tier());
    rc = m0hsm_set_write_tier(id, request.target_tier());
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


        //     struct m0_obj working_obj = motr_obj;
        //     auto rc          = m0hsm_pwrite(
        //         &working_obj, const_cast<void*>(buffer.as_void()),
        //         buffer.length(), offset);
        // // auto rc = m0hsm_test_write();
        //     if (rc < 0) {
        //         std::string msg =
        //             "Error writing buffer at offset " +
        //             std::to_string(offset);
        //         LOG_ERROR(msg);
        //         return {false, 0};
        //     }
        return {false, 0};
        //     return {true, buffer.length()};
    };

    auto sink = InMemoryStreamSink::create(sink_func);
    stream->set_sink(std::move(sink));
}

void MotrInterfaceImpl::get(
    const HsmObjectStoreRequest& request,
    hestia::StorageObject& object,
    hestia::Stream* stream) const
{
    (void)object;

    Uuid uuid;
    uuid.from_string(request.object().id());

    struct m0_uint128 id;
    id.u_hi = uuid.m_hi;
    id.u_lo = uuid.m_lo;

    std::size_t size = request.object().m_size;

    auto source_func = [this, id, size](
                           WriteableBufferView& buffer,
                           std::size_t offset) -> InMemoryStreamSource::Status {
        if (offset >= size) {
            return {true, 0};
        }

        auto read_size = buffer.length();
        if (offset + buffer.length() > size) {
            read_size = size - offset;
        }
        /*
        auto rc = m0hsm_test_read(id, buffer.as_void(), read_size, offset);
        if (rc < 0) {
            std::string msg =
                "Error writing buffer at offset " + std::to_string(offset);
            LOG_ERROR(msg);
            return {false, 0};
        }
        */
        return {true, read_size};
    };

    auto source = hestia::InMemoryStreamSource::create(source_func);
    stream->set_source(std::move(source));
}

void MotrInterfaceImpl::remove(const HsmObjectStoreRequest& request) const
{
    Uuid uuid;
    uuid.from_string(request.object().id());

    struct m0_uint128 id;
    id.u_hi = uuid.m_hi;
    id.u_lo = uuid.m_lo;

    std::size_t offset{0};
    std::size_t length{IMotrInterfaceImpl::max_obj_length};
    enum hsm_rls_flags flags = hsm_rls_flags::HSM_KEEP_LATEST;
    auto rc = m0hsm_release(id, request.source_tier(), offset, length, flags);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_release" + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
}

void MotrInterfaceImpl::copy(const HsmObjectStoreRequest& request) const
{
    Uuid uuid;
    uuid.from_string(request.object().id());

    struct m0_uint128 id;
    id.u_hi = uuid.m_hi;
    id.u_lo = uuid.m_lo;

    std::size_t length = request.extent().m_length;
    if (length == 0) {
        length = IMotrInterfaceImpl::max_obj_length;
    }

    // mock::motr::Hsm::hsm_cp_flags flags =
    // mock::motr::Hsm::hsm_cp_flags::HSM_KEEP_OLD_VERS;
    enum hsm_cp_flags flags = hsm_cp_flags::HSM_KEEP_OLD_VERS;
    auto rc                 = m0hsm_copy(
        id, request.source_tier(), request.target_tier(),
        request.extent().m_offset, length, flags);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_copy" + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
}

void MotrInterfaceImpl::move(const HsmObjectStoreRequest& request) const
{
    Uuid uuid;
    uuid.from_string(request.object().id());

    struct m0_uint128 id;
    id.u_hi = uuid.m_hi;
    id.u_lo = uuid.m_lo;

    std::size_t length = request.extent().m_length;
    if (length == 0) {
        length = IMotrInterfaceImpl::max_obj_length;
    }

    // mock::motr::Hsm::hsm_cp_flags flags =
    // mock::motr::Hsm::hsm_cp_flags::HSM_MOVE;
    enum hsm_cp_flags flags = hsm_cp_flags::HSM_MOVE;
    auto rc                 = m0hsm_copy(
        id, request.source_tier(), request.target_tier(),
        request.extent().m_offset, length, flags);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_copy - move " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
}
}  // namespace hestia
#endif