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
        std::string m_object_id;
        Operation m_op{Operation::GET};
        int m_fd{-1};
        std::size_t m_size{0};
    };

    PhobosDescriptor();

    PhobosDescriptor(const Info& info);

    ~PhobosDescriptor();

    pho_xfer_desc& get_handle();

  private:
    void allocate_handle();
    void clear_object_id();

    void set_up_object_operation(const Info& info);

    std::string m_layout_type{"dir"};
    pho_xfer_desc m_handle;
};

#endif