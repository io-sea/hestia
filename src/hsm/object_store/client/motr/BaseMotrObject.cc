#include "BaseMotrObject.h"

#define MAX_BLOCK_COUNT (200)

namespace hestia{
    BaseMotrObject::BaseMotrObject(const Uuid& id, bool close_on_delete = true) :
        m_uuid(id), m_close_on_delete(close_on_delete)
    {
        set_handle_size();

        // TODO - add proper motr id generation, e.g. following M0_ID_APP
        // restrictions
        set_motr_id();
    }

    BaseMotrObject::~BaseMotrObject()
    {
        LOG_INFO("Clearing object handle: " << m_uuid.to_string());

        if (m_close_on_delete) {
            close_entity();
            //set bool ?
        }
    }

    int BaseMotrObject::read_blocks()
    {
        if (!m_io_ctx) {
            set_IoContext(
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
        return IoCtxt_read_blocks();
    }

    int BaseMotrObject::read(WriteableBufferView& buffer, std::size_t length)
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

    void BaseMotrObject::set_block_layout(std::size_t transfer_size)
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

    std::size_t BaseMotrObject::get_last_transfer_size() const
    {
        return m_unread_block_count * m_block_size;
    }

    void BaseMotrObject::save_size()
    {
        const auto path = std::filesystem::current_path() / "key_value_store"
                          / (UuidUtils::to_string(m_uuid) + ".meta");
        hestia::Metadata data;
        data.set_item("size", std::to_string(m_total_size));
        auto metadata = JsonUtils::to_json(data);
        auto out      = std::ofstream(path);
        out << metadata << std::endl;
    }

    void BaseMotrObject::load_size()
    {
        const auto path = std::filesystem::current_path() / "key_value_store"
                          / (UuidUtils::to_string(m_uuid) + ".meta");
        auto istr = std::ifstream(path);
        std::string value;
        JsonUtils::get_value(path, "size", value);
        m_total_size = std::stol(value);
    }
} //namespace hestia 