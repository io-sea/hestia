#ifdef HAS_MOTR
#include "MotrInterfaceImpl.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "JsonUtils.h"
#include "Logger.h"
#include "UuidUtils.h"

#include "File.h"
#include "UuidUtils.h"

#include <iostream>


#include "motr/idx.h"


#define MAX_BLOCK_COUNT (200)

namespace hestia {
void MotrInterfaceImpl::initialize(const MotrConfig& config)
{
    LOG_INFO("Initializing motr client with config: " << config.to_string());

    m_config = config;

    m_motr_config.mc_is_oostore     = true;
    m_motr_config.mc_is_read_verify = false;

    m_motr_config.mc_local_addr  = m_config.m_local_address.get_value().c_str();
    m_motr_config.mc_ha_addr     = m_config.m_ha_address.get_value().c_str();
    m_motr_config.mc_profile     = m_config.m_profile.get_value().c_str();
    m_motr_config.mc_process_fid = m_config.m_proc_fid.get_value().c_str();

    m_motr_config.mc_tm_recv_queue_min_len = 64;
    m_motr_config.mc_max_rpc_msg_size      = 65536;

    m_motr_config.mc_layout_id = 0;

    m_motr_config.mc_idx_service_id = M0_IDX_DIX;

    m_dix_config.kc_create_meta       = false;
    m_motr_config.mc_idx_service_conf = &m_dix_config;

    const auto rc = m0_client_init(&m_client_instance, &m_motr_config, true);
    if (rc < 0) {
        LOG_ERROR("m0 client init failed: " << rc);
    }

    initialize_hsm(config.m_tier_info.container());
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

    auto input_file    = ::fopen(path.string().c_str(), "r");
    hsm_options.rcfile = input_file;

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

    ::fclose(input_file);
}

class IoContext {
  public:
    IoContext(int num_blocks, size_t block_size, bool alloc_io_buff)
    {
        prepare(num_blocks, block_size, alloc_io_buff);
    }

    ~IoContext() { free(); }

    int prepare(int num_blocks, size_t block_size, bool alloc_io_buff)
    {
        m_allocated_io_buffer = alloc_io_buff;

        /* allocate new I/O vec? */
        if (num_blocks != m_current_blocks
            || block_size != m_current_block_size) {
            if (m_current_blocks != 0) {
                free_data();
            }

            int rc = 0;
            if (m_allocated_io_buffer) {
                rc = m0_bufvec_alloc(&m_data, num_blocks, block_size);
            }
            else {
                rc = m0_bufvec_empty_alloc(&m_data, num_blocks);
            }
            if (rc != 0) {
                return rc;
            }
        }

        /* allocate extents and attrs, if they are not already */
        if (num_blocks != m_current_blocks) {
            /* free previous vectors */
            if (m_current_blocks > 0) {
                free_index();
            }

            /* Allocate attr and extent list*/
            auto rc = m0_bufvec_alloc(&m_attr, num_blocks, 1);
            if (rc != 0) {
                return rc;
            }

            rc = m0_indexvec_alloc(&m_ext, num_blocks);
            if (rc != 0) {
                return rc;
            }
        }

        m_current_blocks     = num_blocks;
        m_current_block_size = block_size;
        return 0;
    }

    int map(int num_blocks, size_t block_size, off_t offset, char* buff)
    {
        if (num_blocks == 0) {
            return -EINVAL;
        }

        for (int i = 0; i < num_blocks; i++) {
            m_ext.iv_index[i]       = offset + i * block_size;
            m_ext.iv_vec.v_count[i] = block_size;

            /* we don't want any attributes */
            m_attr.ov_vec.v_count[i] = 0;

            if (m_data.ov_vec.v_count[i] == 0) {
                m_data.ov_vec.v_count[i] = block_size;
            }
            /* check the allocated buffer has the right size */
            else if (m_data.ov_vec.v_count[i] != block_size) {
                return -EINVAL;
            }

            /* map the user-provided I/O buffer */
            if (buff != nullptr) {
                m_data.ov_buf[i] = buff + i * block_size;
            }
        }
        return 0;
    }

    void free_data()
    {
        if (m_allocated_io_buffer) {
            m0_bufvec_free(&m_data);
        }
        else {
            m0_bufvec_free2(&m_data);
        }
    }

    void free_index()
    {
        m0_bufvec_free(&m_attr);
        m0_indexvec_free(&m_ext);
    }

    void free()
    {
        free_data();
        free_index();
    }

    int do_io_op(struct m0_obj* obj, enum m0_obj_opcode opcode)
    {
        struct m0_op* ops[1] = {NULL};

        /* Create the op request */
        m0_obj_op(obj, opcode, &m_ext, &m_data, &m_attr, 0, 0, &ops[0]);

        /* Launch the op request*/
        m0_op_launch(ops, 1);

        /* wait for completion */
        auto rc =
            m0_op_wait(ops[0], M0_BITS(M0_OS_FAILED, M0_OS_STABLE), M0_TIME_NEVER) ?:
                m0_rc(ops[0]);

        /* finalize and release */
        m0_op_fini(ops[0]);
        m0_op_free(ops[0]);

        return rc;
    }

    int read_blocks(struct m0_obj* obj) { return do_io_op(obj, M0_OC_READ); }

    void to_buffer(WriteableBufferView& buffer, std::size_t block_size)
    {
        std::size_t buff_idx = 0;
        for (std::size_t i = 0; i < m_data.ov_vec.v_nr; i++) {
            for (std::size_t j = 0; j < block_size; j++) {
                buffer.data()[buff_idx] = ((char*)m_data.ov_buf[i])[j];
                buff_idx++;
            }
        }
    }

    int m_current_blocks{0};
    size_t m_current_block_size{0};
    bool m_allocated_io_buffer{false};
    m0_indexvec m_ext;
    m0_bufvec m_data;
    m0_bufvec m_attr;
};

class MotrObject {
  public:
    MotrObject(const Uuid& id, bool close_on_delete = true) :
        m_uuid(id), m_close_on_delete(close_on_delete)
    {
        memset(&m_handle, 0, sizeof(m_handle));
        //        m_uuid=UuidUtils::from_string(id);

        // TODO - add proper motr id generation, e.g. following M0_ID_APP
        // restrictions
        m_motr_id.u_lo += (m_uuid.lo()) | 10000000;
        m_motr_id.u_lo += 10000000;
    }

    ~MotrObject()
    {
        LOG_INFO("Clearing object handle: " << m_uuid.to_string());

        if (m_close_on_delete) {
            m0_entity_fini(&m_handle.ob_entity);
        }
    }

    int read_blocks()
    {
        if (!m_io_ctx) {
            m_io_ctx = std::make_unique<IoContext>(
                m_unread_block_count, m_block_size, true);
        }
        else {
            auto rc =
                m_io_ctx->prepare(m_unread_block_count, m_block_size, true);
            if (rc != 0) {
                return rc;
            }
        }
        auto rc = m_io_ctx->map(
            m_unread_block_count, m_block_size, m_start_offset, nullptr);
        if (rc != 0) {
            return rc;
        }
        return m_io_ctx->read_blocks(&m_handle);
    }

    int read(WriteableBufferView& buffer, std::size_t length)
    {
        for (auto transfer_size = length; transfer_size > 0;
             transfer_size -= get_last_transfer_size()) {
            set_block_layout(transfer_size);

            if (transfer_size < m_min_block_size) {
                m_block_size = m_min_block_size;
            }
            if (auto rc = read_blocks(); rc != 0) {
                return rc;
            }
            if (transfer_size < m_min_block_size) {
                m_io_ctx->to_buffer(buffer, transfer_size);
            }
            else {
                m_io_ctx->to_buffer(buffer, m_block_size);
            }

            m_start_offset += get_last_transfer_size();
        }
        return 0;
    }

    void set_block_layout(std::size_t transfer_size)
    {
        m_unread_block_count = transfer_size / m_block_size;
        if (m_unread_block_count == 0) {
            m_unread_block_count = 1;
            m_block_size         = transfer_size;
        }
        else if (m_unread_block_count > MAX_BLOCK_COUNT) {
            m_unread_block_count = MAX_BLOCK_COUNT;
        }
    }

    std::size_t get_last_transfer_size() const
    {
        return m_unread_block_count * m_block_size;
    }

    void save_size()
    {
        const auto path = std::filesystem::current_path() / "key_value_store"
                          / (UuidUtils::to_string(m_uuid) + ".meta");
        hestia::Map data;
        data.set_item("size", std::to_string(m_total_size));
        auto metadata = JsonUtils::to_json(data);
        auto out      = std::ofstream(path);
        out << metadata << std::endl;
    }

    void load_size()
    {
        const auto path = std::filesystem::current_path() / "key_value_store"
                          / (UuidUtils::to_string(m_uuid) + ".meta");
        auto istr = std::ifstream(path);
        std::string value;
        JsonUtils::get_value(path, "size", value);
        m_total_size = std::stol(value);
    }

    Uuid m_uuid;
    m0_uint128 m_motr_id{M0_ID_APP};
    m0_obj m_handle;
    bool m_close_on_delete{true};

    // Io helpers
    std::size_t m_total_size{0};
    int m_unread_block_count{0};
    std::size_t m_block_size{0};
    std::size_t m_min_block_size{0};
    std::size_t m_start_offset{0};
    std::unique_ptr<IoContext> m_io_ctx;
};

static int open_entity(struct m0_entity* entity)
{
    struct m0_op* ops[1] = {NULL};

    // See above - we should save it as memeber var: hsm_options.op_timeout  =
    // 10;
    int timeout = 10;

    m0_entity_open(entity, &ops[0]);
    m0_op_launch(ops, 1);
    auto rc =
        m0_op_wait(ops[0], M0_BITS(M0_OS_FAILED, M0_OS_STABLE), m0_time_from_now(timeout, 0)) ?:
            m0_rc(ops[0]);
    m0_op_fini(ops[0]);
    m0_op_free(ops[0]);

    return rc;
}

void MotrInterfaceImpl::get(
    const HsmObjectStoreRequest& request,
    hestia::StorageObject& object,
    hestia::Stream* stream) const
{
    LOG_INFO("Starting m0hsm get");

    auto motr_obj = std::make_shared<MotrObject>(
        UuidUtils::from_string(request.object().id()));

    if (request.extent().empty()) {
        motr_obj->m_total_size = request.object().size();
        std::cout
            << "Case request.extent().empty(), request.object().m_size is "
            << request.object().size() << std::endl;
        if (motr_obj->m_total_size == 0) {
            // temporary: load size from temp kv store until full kv store
            // implemented
            motr_obj->load_size();
        }
    }
    else {
        motr_obj->m_start_offset = request.extent().m_offset;
        motr_obj->m_total_size   = request.extent().m_length;
        std::cout
            << "Case request.extent() not empty, request.extent().m_lentgh is "
            << request.extent().m_length << std::endl;
    }
    std::cout << "m_total_size is " << motr_obj->m_total_size << std::endl;
    // motr_obj->m_total_size = 8192;

    m0_obj_init(
        &motr_obj->m_handle, const_cast<m0_realm*>(&m_realm),
        &motr_obj->m_motr_id, m0_client_layout_id(m_client_instance));

    motr_obj->m_min_block_size =
        m0_obj_layout_id_to_unit_size(m0_client_layout_id(m_client_instance));
    motr_obj->m_block_size =
        m0_obj_layout_id_to_unit_size(m0_client_layout_id(m_client_instance));

    auto rc = open_entity(&motr_obj->m_handle.ob_entity);
    if (rc != 0) {
        throw std::runtime_error("Error in hsm get: " + std::to_string(rc));
    }

    auto source_func = [this, motr_obj](
                           WriteableBufferView& buffer,
                           std::size_t offset) -> InMemoryStreamSource::Status {
        if (offset >= motr_obj->m_total_size) {
            return {true, 0};
        }

        auto read_size = buffer.length();
        if (offset + buffer.length() > motr_obj->m_total_size) {
            read_size = motr_obj->m_total_size - offset;
        }

        // read_size = 4096;

        std::cout << "Read size: " << read_size << "\nBuffer size "
                  << buffer.length() << std::endl;

        auto rc = motr_obj->read(buffer, read_size);
        if (rc != 0) {
            return {false, read_size};
        }
        else {
            return {true, read_size};
        }
    };

    auto source = hestia::InMemoryStreamSource::create(source_func);
    stream->set_source(std::move(source));
}

void MotrInterfaceImpl::put(
    const HsmObjectStoreRequest& request, hestia::Stream* stream) const
{
    LOG_INFO("Starting m0hsm put");

    auto motr_obj = std::make_shared<MotrObject>(
        UuidUtils::from_string(request.object().id()));

    LOG_INFO(
        "Creating object with id: [" << motr_obj->m_uuid.to_string() << "]");
    auto rc = m0hsm_create(
        motr_obj->m_motr_id, &motr_obj->m_handle, request.target_tier(), true);
    if (rc < 0) {
        const std::string msg =
            "Failed to create object: " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
    motr_obj->m_block_size =
        m0_obj_layout_id_to_unit_size(m0_client_layout_id(m_client_instance));

    /*
    LOG_INFO("Setting write tier: " << request.target_tier());
    rc = m0hsm_set_write_tier(id, request.target_tier());
    if (rc < 0) {
        const std::string msg =
            "Failed to set write tier: " + std::to_string(rc);
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
    */

    auto sink_func = [this, motr_obj](
                         const ReadableBufferView& buffer,
                         std::size_t offset) -> InMemoryStreamSink::Status {
        LOG_INFO(
            "Writing buffer with size: " << buffer.length() << " and offset "
                                         << offset);

        auto rc = m0hsm_pwrite(
            &motr_obj->m_handle, const_cast<void*>(buffer.as_void()),
            buffer.length(), offset);
        if (rc < 0) {
            std::string msg =
                "Error writing buffer at offset " + std::to_string(offset);
            LOG_ERROR(msg);
            return {false, 0};
        }
        auto num_blocks = (buffer.length()) / (motr_obj->m_block_size);
        if ((buffer.length()) % (motr_obj->m_block_size) != 0) {
            num_blocks++;
        }
        auto write_size = num_blocks * (motr_obj->m_block_size);
        std::cout << write_size << " bytes written" << std::endl;
        motr_obj->m_total_size = write_size;
        // save size (which is an integer multiple of 4096 bytes) that motr has
        // written
        motr_obj->save_size();
        return {true, buffer.length()};
    };

    auto sink = InMemoryStreamSink::create(sink_func);
    stream->set_sink(std::move(sink));
}

void MotrInterfaceImpl::remove(const HsmObjectStoreRequest& request) const
{
    Uuid uuid = UuidUtils::from_string(request.object().id());
    // uuid=UuidUtils::from_string(request.object().id());

    struct m0_uint128 id;
    id.u_hi = uuid.hi();
    id.u_lo = uuid.lo();

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
    Uuid uuid = UuidUtils::from_string(request.object().id());

    struct m0_uint128 id;
    id.u_hi = uuid.hi();
    id.u_lo = uuid.lo();

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
    Uuid uuid = UuidUtils::from_string(request.object().id());
    // uuid.from_string(request.object().id());

    struct m0_uint128 id;
    id.u_hi = uuid.hi();
    id.u_lo = uuid.lo();

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