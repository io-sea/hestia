#include "SystemUtils.h"

#include <string.h>

#include <limits.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <net/if.h>
#ifndef __linux__
#include <net/if_dl.h>
#endif
#include <netdb.h>

#include <netinet/in.h>

#include <ifaddrs.h>

#include <dlfcn.h>

#include "Logger.h"

#ifndef HOST_NAME_MAX
#ifdef _POSIX_HOST_NAME_MAX
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#else
#define HOST_NAME_MAX 255
#endif
#endif


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

#ifdef __linux__
std::pair<OpStatus, std::string> get_mac_address_linux()
{
    OpStatus status;

    errno     = 0;
    auto sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) {
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_code    = errno;
        status.m_error_message = ::strerror(status.m_error_code);
        return {status, {}};
    };

    char buf[1024];
    ifconf ifc;
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    errno       = 0;
    if (::ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_code    = errno;
        status.m_error_message = ::strerror(status.m_error_code);
        return {status, {}};
    }

    auto iter      = ifc.ifc_req;
    const auto end = iter + (ifc.ifc_len / sizeof(struct ifreq));

    int success = 0;
    ifreq ifr;
    for (; iter != end; ++iter) {
        ::strncpy(ifr.ifr_name, iter->ifr_name, IFNAMSIZ);
        errno = 0;
        if (::ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            // don't count loopback
            if ((ifr.ifr_flags & IFF_LOOPBACK) != 0) {
                if (::ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    success = 1;
                    break;
                }
            }
        }
        else {
            status.m_status        = OpStatus::Status::ERROR;
            status.m_error_code    = errno;
            status.m_error_message = ::strerror(status.m_error_code);
            return {status, {}};
        }
    }

    std::vector<unsigned char> mac_address(6, 0);
    if (success != 0) {
        ::memcpy(mac_address.data(), ifr.ifr_hwaddr.sa_data, 6);
        return {status, std::string(mac_address.begin(), mac_address.end())};
    }
    else {
        status.m_status     = OpStatus::Status::ERROR;
        status.m_error_code = -1;
        status.m_error_message =
            "Failed to find suitable interface for mac address lookup";
        return {status, {}};
    }
}
#else

static std::pair<OpStatus, std::string> get_mac_address_mac()
{
    OpStatus status;

    ifaddrs* ifap{nullptr};
    errno = 0;
    if (::getifaddrs(&ifap) == 0) {
        std::string address;
        for (auto ifaptr = ifap; ifaptr != nullptr;
             ifaptr      = (ifaptr)->ifa_next) {
            if (ifaptr->ifa_addr == nullptr) {
                continue;
            }
            bool is_loopback = (ifaptr->ifa_flags & IFF_LOOPBACK) != 0;
            if (ifaptr->ifa_addr->sa_family == AF_LINK && !is_loopback) {
                char host[NI_MAXHOST];
                ::getnameinfo(
                    ifaptr->ifa_addr, sizeof(struct sockaddr_dl), host,
                    NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
                address = std::string(host);
                if (!address.empty()) {
                    break;
                }
            }
        }
        ::freeifaddrs(ifap);

        if (address.empty()) {
            status.m_status     = OpStatus::Status::ERROR;
            status.m_error_code = -1;
            status.m_error_message =
                "Failed to find suitable interface for mac address lookup";
            return {status, {}};
        }
        else {
            return {status, address};
        }
    }
    else {
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_code    = errno;
        status.m_error_message = ::strerror(status.m_error_code);
        return {status, {}};
    }
}
#endif

std::pair<OpStatus, std::string> SystemUtils::get_mac_address()
{
#ifdef __linux__
    return get_mac_address_linux();
#else
    return get_mac_address_mac();
#endif
}

std::pair<OpStatus, std::string> SystemUtils::get_hostname()
{
    OpStatus status;
    std::vector<char> buffer(HOST_NAME_MAX + 1, 0);
    errno = 0;
    if (::gethostname(buffer.data(), HOST_NAME_MAX) != 0) {
        status.m_status        = OpStatus::Status::ERROR;
        status.m_error_code    = errno;
        status.m_error_message = ::strerror(status.m_error_code);
        return {status, {}};
    }
    else {
        auto ret = std::string(buffer.begin(), buffer.end());
        ret.erase(std::find(ret.begin(), ret.end(), '\0'), ret.end());
        return {status, ret};
    }
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