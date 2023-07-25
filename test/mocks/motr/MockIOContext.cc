#include "MockIOContext.h"
#include "MockMotrObject.h"

namespace hestia {

    MockIoContext::MockIoContext(int num_blocks, size_t block_size, bool alloc_io_buff): IoContext(num_blocks, block_size, alloc_io_buff){}

     std::size_t MockIoContext::get_extent_index(int i){ return m_ext.m_indices[i]; }
     std::size_t MockIoContext::get_extent_count(int i){ return m_ext.m_counts[i]; }
     std::size_t MockIoContext::get_attr_count(int i){ return m_attr.m_counts[i]; }
     std::size_t MockIoContext::get_data_count(int i){ return m_data.m_counts[i];}
     std::size_t MockIoContext::get_data_size(){return m_data.m_counts.size();}
     void* MockIoContext::get_data_buf(int i){return m_data.m_buffers[i];}
     void MockIoContext::set_extent_index(int i, size_t n){m_ext.m_indices[i] = n;}
     void MockIoContext::set_extent_count(int i, size_t n){m_ext.m_counts[i]=n;}
     void MockIoContext::set_attr_count(int i, size_t n){m_attr.m_counts[i]=n;}
     void MockIoContext::set_data_count(int i, size_t n){m_data.m_counts[i]=n;}
     void MockIoContext::set_data_buf(int i, char * buf){m_data.m_buffers[i] = buf;}
     void MockIoContext::indexvec_free(){m_ext.clear();}
     void MockIoContext::attrvec_free(){
        for(std::size_t i =0; i<m_attr.m_buffers.size(); i++){
            delete [] m_attr.m_buffers[i];
		}
        m_attr.clear();
        }
     void MockIoContext::buffvec_free(){
        for(std::size_t i =0; i<m_data.m_buffers.size(); i++){
            delete [] m_data.m_buffers[i];
		}
        m_data.clear();
        m_memory_allocated= false;
     }
     void MockIoContext::buffvec_free2(){m_data.clear();}
     int MockIoContext::alloc_data(int num_blocks, std::size_t block_size){
        m_data.m_buffers.resize(num_blocks);
        m_data.m_counts.resize(num_blocks);
        m_memory_allocated= true;
        for(int i =0; i<num_blocks; i++){
			m_data.m_buffers[i] = new char[block_size];
		}

        return 0; /*memory allocated flag, resize vector*/
    }
     int MockIoContext::empty_alloc_data(int num_blocks){
        m_data.m_buffers.resize(num_blocks);
        m_data.m_counts.resize(num_blocks);
        return 0;
    }
     int MockIoContext::alloc_attr(int num_blocks, std::size_t block_size){
        m_attr.m_buffers.resize(num_blocks);
        m_attr.m_counts.resize(num_blocks);
        for(int i =0; i<num_blocks; i++){
			m_attr.m_buffers[i] = new char[block_size];
		}

        return 0;
     }
     int MockIoContext::index_alloc(int num_blocks){
        m_ext.m_counts.resize(num_blocks);
        m_ext.m_indices.resize(num_blocks);
        return 0;
        }

    int MockIoContext::read_blocks(MockMotrObject* Obj)
    {   
        for(std::size_t i=0; i<m_data.m_counts.size(); i++){
            int idx=m_ext.m_indices[i];
            for(std::size_t j=0; j<m_data.m_counts[i];j++){
                ((char*)m_data.m_buffers[i])[j] = (Obj->m_stored)[idx + j];  
            }    
        }   
        return 0;
    }

}