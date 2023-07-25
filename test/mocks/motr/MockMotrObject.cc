#include "MockMotrObject.h"
#include "MockIOContext.h"

namespace hestia{

    MockMotrObject::MockMotrObject(const Uuid& id, bool close_on_delete): BaseMotrObject(id, close_on_delete){}

    void MockMotrObject::set_handle_size(){return ;}
    void MockMotrObject::set_motr_id(){
        m_motr_id.m_lo = m_uuid.m_lo;
        m_motr_id.m_hi = m_uuid.m_hi;

    }
    void MockMotrObject::close_entity(){return ;}


    int MockMotrObject::IoCtxt_read_blocks(){return m_io_ctx->read_blocks(this);}


    void MockMotrObject::set_IoContext(int num_blocks, size_t block_size, bool alloc_io_buff)
        {m_io_ctx = std::make_unique<MockIoContext>(
                m_unread_block_count, m_block_size, true);}

}