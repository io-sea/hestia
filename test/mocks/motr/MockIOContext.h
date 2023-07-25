#pragma once

#include "IOContext.h"
#include "MockMotrTypes.h"

namespace hestia{

class MockIoContext: public IoContext {
  public:
  
    MockIoContext(int num_blocks, size_t block_size, bool alloc_io_buff);

    std::size_t get_extent_index(int i);
     std::size_t get_extent_count(int i);
     std::size_t get_attr_count(int i);
     std::size_t get_data_count(int i);
     std::size_t get_data_size();
     void* get_data_buf(int i);
     void set_extent_index(int i, size_t n);
     void set_extent_count(int i, size_t n);
     void set_attr_count(int i, size_t n);
     void set_data_count(int i, size_t n);
     void set_data_buf(int i, char * buf);
     void indexvec_free();
     void attrvec_free();
     void buffvec_free();
     void buffvec_free2(){m_data.clear();}
     int alloc_data(int num_blocks, std::size_t block_size);
     int empty_alloc_data(int num_blocks);
     int alloc_attr(int num_blocks, std::size_t block_size);
     int index_alloc(int num_blocks);

    struct mock::motr::IndexVec m_ext;
    struct mock::motr::BufferVec m_data;
    struct mock::motr::BufferVec m_attr;
    bool m_memory_allocated = false;


};

} //namespace hestia