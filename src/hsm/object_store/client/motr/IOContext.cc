#include "IOContext.h"

namespace hestia {
    IoContext::IoContext(int num_blocks, size_t block_size, bool alloc_io_buff)
    {
        prepare(num_blocks, block_size, alloc_io_buff);
    }

    int IoContext::prepare(int num_blocks, size_t block_size, bool alloc_io_buff)
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
                rc = alloc_data(num_blocks, block_size);
            }
            else {
                rc = empty_alloc_data(num_blocks);
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
            auto rc = alloc_attr(num_blocks, 1);
            if (rc != 0) {
                return rc;
            }

            rc = index_alloc(num_blocks);
            if (rc != 0) {
                return rc;
            }
        }

        m_current_blocks     = num_blocks;
        m_current_block_size = block_size;
        return 0;
    }

    int IoContext::map(int num_blocks, size_t block_size, off_t offset, char* buff)
    {
        if (num_blocks == 0) {
            return -EINVAL;
        }

        for (int i = 0; i < num_blocks; i++) {
            set_extent_index(i, offset + i * block_size);
            set_extent_count(i, block_size);

            /* we don't want any attributes */
            set_attr_count(i, 0);

            if (get_data_count(i) == 0) {
                set_data_count(i, block_size);
            }
            /* check the allocated buffer has the right size */
            else if (get_data_count(i) != block_size) {
                return -EINVAL;
            }

            /* map the user-provided I/O buffer */
            if (buff != nullptr) {
                set_data_buf(i, buff + i * block_size);
            }
        }
        return 0;
    }  

    void IoContext::free_data()
    {
        if (m_allocated_io_buffer) {   
            buffvec_free();
        }
        else {
            buffvec_free2();
        }
    }

    void IoContext::free_index()
    {
        attrvec_free();
        indexvec_free();
    }

    void IoContext::free()
    {
        free_data();
        free_index();
    }

    void IoContext::to_buffer(WriteableBufferView& buffer, std::size_t block_size)
    {
        std::size_t buff_idx = 0;
        for (std::size_t i = 0; i < get_data_size(); i++) {
            for (std::size_t j = 0; j < block_size; j++) {
                buffer.data()[buff_idx] = ((char*)get_data_buf(i))[j];
                buff_idx++;
            }
        }
    }

 
}//namespace hestia 