#pragma once

#include "UuidUtils.h"
#include "WriteableBufferView.h"

namespace hestia{

class BaseMotrObject;

class IoContext {
  public:
    IoContext(int num_blocks, size_t block_size, bool alloc_io_buff);

    virtual ~IoContext() { free(); };

    int prepare(int num_blocks, size_t block_size, bool alloc_io_buff);

    int map(int num_blocks, size_t block_size, off_t offset, char* buff);

    void free_data();

    void free_index();

    void free();

    void to_buffer(WriteableBufferView& buffer, std::size_t block_size);

    virtual int read_blocks(BaseMotrObject* Obj)=0;

    virtual std::size_t get_extent_index(int i);
    virtual std::size_t get_extent_count(int i);
    virtual std::size_t get_attr_count(int i);
    virtual std::size_t get_data_count(int i);
    virtual std::size_t get_data_size();
    virtual void* get_data_buf(int i);
    virtual void set_extent_index(int i, size_t n);
    virtual void set_extent_count(int i, size_t n);
    virtual void set_attr_count(int i, size_t n);
    virtual void set_data_count(int i, size_t n);
    virtual void set_data_buf(int i, char * buf);
    virtual void indexvec_free();
    virtual void attrvec_free();
    virtual void buffvec_free();
    virtual void buffvec_free2();
    virtual int alloc_data(int num_blocks, std::size_t block_size);
    virtual int empty_alloc_data(int num_blocks);
    virtual int alloc_attr(int num_blocks, std::size_t block_size);
    virtual int index_alloc(int num_blocks);

    int m_current_blocks{0};
    std::size_t m_current_block_size{0};
    bool m_allocated_io_buffer{false};
};
} //namespace hestia