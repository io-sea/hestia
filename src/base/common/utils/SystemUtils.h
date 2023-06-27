#pragma once

#include "ErrorUtils.h"

#include <utility>

namespace hestia {

using handle_t = int;

class SystemUtils {
  public:
    static OpStatus close_files(handle_t file_descriptors[], std::size_t count);

    static OpStatus do_close(handle_t& file_descriptor);

    static void close_standard_file_descriptors();

    static OpStatus open_pipe(handle_t file_descriptors[]);

    static std::pair<OpStatus, std::string> get_mac_address();

    static std::pair<OpStatus, std::string> get_hostname();

    static std::pair<OpStatus, std::size_t> do_read(
        handle_t file_descriptor, char* buffer, std::size_t size);

    static std::pair<OpStatus, std::size_t> do_write(
        handle_t file_descriptor, const char* buffer, std::size_t size);

    static std::pair<OpStatus, void*> load_module(const std::string& path);

    static OpStatus close_module(void* handle, const std::string& name);

    static std::pair<OpStatus, void*> load_symbol(
        void* handle, const std::string& module_name, const std::string& name);
};
}  // namespace hestia