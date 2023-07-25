#pragma once

#include "BaseMotrObject.h"
#include "MockMotrTypes.h"

namespace hestia{
class MockMotrObject: public BaseMotrObject {
 public:
    
    MockMotrObject(const Uuid& id, bool close_on_delete = true);

    static mock::motr::Id to_motr_id(const hestia::Uuid& id);


    void set_handle_size();
    void set_motr_id();
    void close_entity();
    int IoCtxt_read_blocks();
    void set_IoContext(int num_blocks, size_t block_size, bool alloc_io_buff);

    std::size_t m_size{0};
    mock::motr::Id m_motr_id;
    mock::motr::Obj m_handle;
    void * m_stored;
};
}//namespace hestia