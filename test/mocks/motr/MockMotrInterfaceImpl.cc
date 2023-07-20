#include "MockMotrInterfaceImpl.h"
#include "IOContext.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "Logger.h"

#include <stdexcept>

#define MAX_BLOCK_COUNT (200)

namespace hestia {
    
class MockIoContext: public IoContext {
  public:
    std::size_t get_extent_index(int i){ return m_ext.m_indices[i]; }
     std::size_t get_extent_count(int i){ return m_ext.m_counts[i]; }
     std::size_t get_attr_count(int i){ return m_attr.m_counts[i]; }
     std::size_t get_data_count(int i){ return m_data.m_counts[i];}
     std::size_t get_data_size(){return m_data.m_counts.size();}
     void* get_data_buf(int i){return m_data.m_buffers[i];}
     void set_extent_index(int i, size_t n){m_ext.m_indices[i] = n;}
     void set_extent_count(int i, size_t n){m_ext.m_counts[i]=n;}
     void set_attr_count(int i, size_t n){m_attr.m_counts[i]=n;}
     void set_data_count(int i, size_t n){m_data.m_counts[i]=n;}
     void set_data_buf(int i, char * buf){m_data.m_buffers[i] = buf;}
     void indexvec_free(){m_attr.clear();}
     void attrvec_free(){m_attr.clear();}
     void buffvec_free(){m_data.clear();}
     void buffvec_free2(){m_data.clear();}
     int alloc_data(int num_blocks, std::size_t block_size){return 0;}
     int empty_alloc_data(int num_blocks){return 0;}
     int alloc_attr(int num_blocks, std::size_t block_size){return 0;}
     int index_alloc(int num_blocks){return 0;}
/*
    int do_io_op(struct m0_obj* obj, enum m0_obj_opcode opcode)
    {
        struct m0_op* ops[1] = {NULL};

        /* Create the op request */
        //m0_obj_op(obj, opcode, &m_ext, &m_data, &m_attr, 0, 0, &ops[0]);

        /* Launch the op request*/
        //m0_op_launch(ops, 1);

        /* wait for completion */
        //auto rc = 0;
        //    m0_op_wait(ops[0], M0_BITS(M0_OS_FAILED, M0_OS_STABLE), M0_TIME_NEVER) ?:
        //        m0_rc(ops[0]);

        /* finalize and release *
        //m0_op_fini(ops[0]);
        //m0_op_free(ops[0]);

        return rc;
    }
*/
    //int read_blocks(struct m0_obj* obj) { return do_io_op(obj, M0_OC_READ); }
    //int read_blocks(mock::motr::Obj* obj){return mock::motr::MotrBackend::read_object(obj,m_ext,m_data);}

    //TODO add the mock read/write blocks methods

    struct mock::motr::IndexVec m_ext;
    struct mock::motr::BufferVec m_data;
    struct mock::motr::BufferVec m_attr;
};

class MotrObject {
  public:
    MotrObject(const hestia::Uuid& oid) : m_id(oid) {}


    ~MotrObject() = default;

    mock::motr::Id get_motr_id() const { return to_motr_id(m_id); }

    mock::motr::Obj* get_motr_obj() { return &m_handle; }

    static mock::motr::Id to_motr_id(const hestia::Uuid& id)
    {
        mock::motr::Id motr_id;
        motr_id.m_lo = id.m_lo;
        motr_id.m_hi = id.m_hi;
        return motr_id;
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

            // if (transfer_size < m_min_block_size) {
            //     m_block_size = m_min_block_size;
            // }
            if (auto rc = read_blocks(); rc != 0) {
                return rc;
            }
            if (transfer_size < m_block_size) {
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
            //    m_block_size         = transfer_size;
        }
        else if (m_unread_block_count > MAX_BLOCK_COUNT) {
            m_unread_block_count = MAX_BLOCK_COUNT;
        }
    }

    std::size_t get_last_transfer_size() const
    {
        return m_unread_block_count * m_block_size;
    }

    std::size_t m_size{0};

  private:
    hestia::Uuid m_id;
    mock::motr::Obj m_handle;

    std::size_t m_total_size{0};
    int m_unread_block_count{0};
    std::size_t m_block_size{0};
    std::size_t m_start_offset{0};
    std::unique_ptr<IoContext> m_io_ctx;
};


void MockMotrInterfaceImpl::initialize(const MotrConfig& config)
{
    static constexpr int mo_uber_realm = 0;

    m_hsm.motr()->m0_client_init(&m_client, nullptr, true);
    mock::motr::Id realm_id{mo_uber_realm, 0};
    m_hsm.motr()->m0_container_init(
        &m_container, nullptr, &realm_id, &m_client);

    std::size_t pool_count{0};
    LOG_INFO("Initializing with: " << config.m_tier_info.size() << " pools.");
    for (const auto& entry : config.m_tier_info) {
        (void)entry;
        m_client.add_pool({pool_count, 0});
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
        pool_fids.push_back({pool_count, 0});
        pool_count++;
    }

    mock::motr::HsmOptions hsm_options;
    hsm_options.m_pool_fids = pool_fids;

    m_hsm.m0hsm_init(&m_client, &m_container.m_co_realm, &hsm_options);
}

void MockMotrInterfaceImpl::put(
    const HsmObjectStoreRequest& request, hestia::Stream* stream) const
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
}

void MockMotrInterfaceImpl::get(
    const HsmObjectStoreRequest& request,
    hestia::StorageObject& object,
    hestia::Stream* stream) const
{
    (void)object;

    MotrObject motr_obj(request.object().id());
    motr_obj.m_size = request.object().m_size;

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

    auto source = hestia::InMemoryStreamSource::create(source_func);
    stream->set_source(std::move(source));
}

void MockMotrInterfaceImpl::remove(const HsmObjectStoreRequest& request) const
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
}

void MockMotrInterfaceImpl::copy(const HsmObjectStoreRequest& request) const
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
}

void MockMotrInterfaceImpl::move(const HsmObjectStoreRequest& request) const
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
}
}  // namespace hestia