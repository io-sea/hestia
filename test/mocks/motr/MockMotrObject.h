#include "BaseMotrObject.h"
#include "MockMotrTypes.h"

namespace hestia{
class MockMotrObject: public BaseMotrObject {

    void set_handle_size();
    void set_motr_id();
    void close_entity();
    int IoCtxt_read_blocks();
    void set_IoContext(int num_blocks, size_t block_size, bool alloc_io_buff);

    std::size_t m_size{0};
    mock::motr::Obj m_handle;
    void * handle;
};
}//namespace hestia