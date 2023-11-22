#pragma once

#if HAS_PHOBOS
#include <memory>
#include <string>

extern "C" {
#include "PhobosWrapper.h"
}

class PhobosDescriptor {
  public:
    enum class Operation { GET, PUT, GET_MD, DEL };

    struct Info {
        Info(
            const std::string& object_id,
            Operation op,
            int fd           = -1,
            std::size_t size = 0) :
            m_object_id(object_id), m_op(op), m_fd(fd), m_size(size)
        {
        }

        std::string m_object_id;
        Operation m_op{Operation::GET};
        int m_fd{-1};
        std::size_t m_size{0};
    };

    PhobosDescriptor();

    PhobosDescriptor(const Info& info);

    ~PhobosDescriptor();

    void set_fd(int fd);

    void set_size(std::size_t size);

    pho_xfer_desc& get_handle();

  private:
    void allocate_handle();
    void clear_object_id();

    void set_up_object_operation(const Info& info);

    rsc_family m_layout_type{rsc_family::PHO_RSC_DIR};
    pho_xfer_desc m_handle;
};

#endif