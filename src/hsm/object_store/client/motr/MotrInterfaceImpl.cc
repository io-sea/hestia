#include "MotrInterfaceImpl.h"

#include "Logger.h"

#include <iostream>

#ifdef HAS_MOTR
namespace hestia {
void MotrInterfaceImpl::initialize(const MotrConfig& config)
{
    m_config = config;

    m0_config m0tr_config;
    m0tr_config.mc_is_oostore     = true;
    m0tr_config.mc_is_read_verify = false;

    m0tr_config.mc_local_addr  = m_config.m_local_address.c_str();
    m0tr_config.mc_ha_addr     = m_config.m_ha_address.c_str();
    m0tr_config.mc_profile     = m_config.m_profile.c_str();
    m0tr_config.mc_process_fid = m_config.m_proc_fid.c_str();

    const auto rc = m0_client_init(&m_client_instance, &m0tr_config, true);
    if (rc < 0) {
        LOG_ERROR("m0 client init failed: " << rc);
    }
}

void MotrInterfaceImpl::finish()
{
    if (m_client_instance) {
        m0_client_fini(m_client_instance, false);
        m_client_instance = nullptr;
    }
}

void MotrInterfaceImpl::initialize_hsm()
{
    m0hsm_options hsm_options;
    hsm_options.trace_level = LOG_INFO;
    hsm_options.op_timeout  = 10;
    hsm_options.log_stream  = stderr;

    if (!std::filesystem::is_regular_file(m_config.m_hsm_config_path)) {
        LOG_ERROR("No hsm config file found - bailing out.");
        return;
    }

    FILE* f_in         = ::fopen(m_config.m_hsm_config_path.c_str(), "r");
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

int MotrInterfaceImpl::put(
    const HsmObjectStoreRequest& request, hestia::Stream* stream)
{
    Uuid id;
    id.from_string(request.object().id());

    MotrObject motr_obj(id);

    auto rc = m0hsm_create(
        motr_obj.get_motr_id(), motr_obj.get_motr_obj(), request.target_tier(),
        false);
    if (rc < 0) {
        const std::string msg =
            "Failed to create object: " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }

    rc = m0hsm_set_write_tier(motr_obj.get_motr_id(), request.target_tier());
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
        auto rc          = m0hsm_pwrite(
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
}

int MotrInterfaceImpl::get(
    const HsmObjectStoreRequest& request,
    hestia::StorageObject& object,
    hestia::Stream* stream)
{
    (void)object;

    Uuid id;
    id.from_string(request.object().id());
    MotrObject motr_obj(id);
    motr_obj.m_size = request.object().m_size;

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

        auto rc = m0hsm_read(
            motr_obj.get_motr_id(), buffer.as_void(), read_size, offset);
        if (rc < 0) {
            std::string msg =
                "Error writing buffer at offset " + std::to_string(offset);
            LOG_ERROR(msg);
            return {false, 0};
        }
        return {true, read_size};
    };

    auto source = hestia::InMemoryStreamSource::create(source_func);
    stream->set_source(std::move(source));
}

int MotrInterfaceImpl::remove(const HsmObjectStoreRequest& request)
{
    Uuid id;
    id.from_string(request.object().id());

    MotrObject motr_obj(id);

    std::size_t offset{0};
    std::size_t length{IMotrInterfaceImpl::max_obj_length};
    auto rc = m0hsm_release(
        motr_obj.get_motr_id(), request.source_tier(), offset, length, 0);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_release" + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
}

int MotrInterfaceImpl::copy(const HsmObjectStoreRequest& request)
{
    Uuid id;
    id.from_string(request.object().id());

    MotrObject motr_obj(id);
    std::size_t length = request.extent().m_length;
    if (length == 0) {
        length = IMotrInterfaceImpl::max_obj_length;
    }

    // mock::motr::Hsm::hsm_cp_flags flags =
    // mock::motr::Hsm::hsm_cp_flags::HSM_KEEP_OLD_VERS;
    auto rc = m0hsm_copy(
        motr_obj.get_motr_id(), request.source_tier(), request.target_tier(),
        request.extent().m_offset, length, 0);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_copy" + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
}

int MotrInterfaceImpl::move(const HsmObjectStoreRequest& request)
{
    Uuid id;
    id.from_string(request.object().id());

    MotrObject motr_obj(id);
    std::size_t length = request.extent().m_length;
    if (length == 0) {
        length = IMotrInterfaceImpl::max_obj_length;
    }

    // mock::motr::Hsm::hsm_cp_flags flags =
    // mock::motr::Hsm::hsm_cp_flags::HSM_MOVE;
    auto rc = m_hsm.m0hsm_copy(
        motr_obj.get_motr_id(), request.source_tier(), request.target_tier(),
        request.extent().m_offset, length, 0);
    if (rc < 0) {
        std::string msg = "Error in  m0hsm_copy - move " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
}
}  // namespace hestia
#endif