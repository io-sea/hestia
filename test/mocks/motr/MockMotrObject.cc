#include "MockMotrObject.h"

namespace hestia{

    void MockMotrObject::set_handle_size(){return ;}
    void MockMotrObject::set_motr_id(){return ;}
    void MockMotrObject::close_entity(){return ;}
    int MockMotrObject::IoCtxt_read_blocks(){return m_io_ctx->read_blocks((char*) handle);}
    void MockMotrObject::set_IoContext(int num_blocks, size_t block_size, bool alloc_io_buff)
        {m_io_ctx = std::make_unique<MockIoContext>(
                m_unread_block_count, m_block_size, true);}

}