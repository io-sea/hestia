#include "SystemUtils.h"

#include <string.h>
#include <unistd.h>

#include <dlfcn.h>

namespace hestia {

std::pair<OpStatus, void*> SystemUtils::load_symbol(
    void* handle, const std::string& module_name, const std::string& name)
{
    OpStatus status;
    auto symbol = ::dlsym(handle, name.c_str());
    if (symbol == nullptr) {
        std::string msg{
            "Error loading symbol '" + name + "' from: " + module_name
            + " | reason: "};
        if (auto error = ::dlerror()) {
            msg += std::string(error);
        }
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_message = msg;
        return {status, nullptr};
    }
    return {status, symbol};
}

std::pair<OpStatus, void*> SystemUtils::load_module(const std::string& path)
{
    OpStatus status;

    auto handle = ::dlopen(path.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        std::string msg{"Error loading plugin from: " + path + " | reason: "};
        if (auto error = ::dlerror()) {
            msg += std::string(error);
        }
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_message = msg;
        return {status, nullptr};
    }
    return {status, handle};
}

OpStatus SystemUtils::close_module(void* handle, const std::string& name)
{
    OpStatus status;
    if (handle != nullptr) {
        if (const auto close_status = ::dlclose(handle); close_status != 0) {
            std::string msg{"Error closing plugin: " + name + " | reason: "};
            if (auto error = ::dlerror()) {
                msg += std::string(error);
            }
            status.m_status        = OpStatus::Status::ERROR;
            status.m_error_message = msg;
        }
    }
    return status;
}

OpStatus SystemUtils::open_pipe(int file_descriptors[])
{
    OpStatus status;
    errno         = 0;
    const auto rc = ::pipe(file_descriptors);
    if (rc == -1) {
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_code    = errno;
        status.m_error_message = ::strerror(status.m_error_code);
    }
    return status;
}

OpStatus SystemUtils::do_close(handle_t& fd)
{
    OpStatus status;
    errno         = 0;
    const auto rc = ::close(fd);
    if (rc == -1) {
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_code    = errno;
        status.m_error_message = ::strerror(status.m_error_code);
    }
    else {
        fd = -1;
    }
    return status;
}

OpStatus SystemUtils::close_files(int file_descriptors[], std::size_t count)
{
    OpStatus status;
    for (std::size_t idx = 0; idx < count; idx++) {
        if (file_descriptors[idx] > 0) {
            errno         = 0;
            const auto rc = ::close(file_descriptors[idx]);
            if (rc == -1) {
                status.m_status        = OpStatus::Status::ERROR;
                status.m_error_code    = errno;
                status.m_error_message = ::strerror(status.m_error_code);
            }
            else {
                file_descriptors[0] = -1;
            }
        }
    }
    return status;
}

void SystemUtils::close_standard_file_descriptors()
{
    ::close(STDIN_FILENO);
    ::close(STDOUT_FILENO);
    ::close(STDERR_FILENO);
}

std::pair<OpStatus, std::size_t> SystemUtils::do_write(
    int fd, const char* buffer, std::size_t size)
{
    OpStatus status;
    errno         = 0;
    const auto rc = ::write(fd, buffer, size);
    std::size_t num_written{0};
    if (rc < 0) {
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_code    = errno;
        status.m_error_message = ::strerror(status.m_error_code);
    }
    else {
        num_written = rc;
    }
    return {status, num_written};
}

std::pair<OpStatus, std::size_t> SystemUtils::do_read(
    int fd, char* buffer, std::size_t size)
{
    OpStatus status;
    errno         = 0;
    const auto rc = ::read(fd, buffer, size);
    std::size_t num_read{0};
    if (rc < 0) {
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_code    = errno;
        status.m_error_message = ::strerror(status.m_error_code);
    }
    else {
        num_read = rc;
    }
    return {status, num_read};
}

}  // namespace hestia