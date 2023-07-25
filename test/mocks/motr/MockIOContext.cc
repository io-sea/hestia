#include "MockIOContext.h"

namespace hestia {

    MockIoContext::MockIoContext(int num_blocks, size_t block_size, bool alloc_io_buff): IoContext(num_blocks, block_size, alloc_io_buff){}

     std::size_t MockIOContext::get_extent_index(int i){ return m_ext.m_indices[i]; }
     std::size_t MockIOContext::get_extent_count(int i){ return m_ext.m_counts[i]; }
     std::size_t MockIOContext::get_attr_count(int i){ return m_attr.m_counts[i]; }
     std::size_t MockIOContext::get_data_count(int i){ return m_data.m_counts[i];}
     std::size_t MockIOContext::get_data_size(){return m_data.m_counts.size();}
     void* MockIOContext::get_data_buf(int i){return m_data.m_buffers[i];}
     void MockIOContext::set_extent_index(int i, size_t n){m_ext.m_indices[i] = n;}
     void MockIOContext::set_extent_count(int i, size_t n){m_ext.m_counts[i]=n;}
     void MockIOContext::set_attr_count(int i, size_t n){m_attr.m_counts[i]=n;}
     void MockIOContext::set_data_count(int i, size_t n){m_data.m_counts[i]=n;}
     void MockIOContext::set_data_buf(int i, char * buf){m_data.m_buffers[i] = buf;}
     void MockIOContext::indexvec_free(){m_ext.clear();}
     void MockIOContext::attrvec_free(){m_attr.clear();}
     void MockIOContext::buffvec_free(){
        m_data.clear();
        m_memory_allocated= false;
     }
     void MockIOContext::buffvec_free2(){m_data.clear();}
     int MockIOContext::alloc_data(int num_blocks, std::size_t block_size){
        m_data.m_buffers.resize(num_blocks);
        m_data.m_counts.resize(num_blocks);
        m_memory_allocated= true;
        return 0; /*memory allocated flag, resize vector*/
    }
     int MockIOContext::empty_alloc_data(int num_blocks){
        m_data.m_buffers.resize(num_blocks);
        m_data.m_counts.resize(num_blocks);
        return 0;
    }
     int MockIOContext::alloc_attr(int num_blocks, std::size_t block_size){
        m_attr.m_buffers.resize(num_blocks);
        m_attr.m_counts.resize(num_blocks);
        return 0;
     }
     int MockIOContext::index_alloc(int num_blocks){
        m_ext.m_counts.resize(num_blocks);
        m_ext.m_indices.resize(num_blocks);
        return 0;
        }

    int MockIOContext::read_blocks(MockMotrObject* Obj)
    {   
        auto stored = Obj->m_stored;
        for(std::size_t i=0; i<m_data.m_counts.size(); i++){
            int idx=m_ext.m_indices[i];
            for(std::size_t j=0; j<m_data.m_counts[i];j++){
                ((char*)m_data.m_buffers[i])[j] = stored[idx + j];  
            }    
        }   
        return 0;
    }

}