#include "IdGenerator.h"

#include "HashUtils.h"
#include "SystemUtils.h"
#include "TimeUtils.h"
#include "UuidUtils.h"

#include <stdexcept>

namespace hestia {
Uuid DefaultIdGenerator::get_uuid(const std::string& key)
{
    std::string address_key;
    const auto& [status, mac_address] = SystemUtils::get_mac_address();
    if (!status.ok()) {
        const auto& [host_status, host_address] = SystemUtils::get_hostname();
        {
            address_key = host_address;
        }
    }
    address_key = mac_address;

    const auto hash = HashUtils::do_md5(
        key + std::to_string(TimeUtils::get_current_time()) + address_key);
    return UuidUtils::from_string(hash, Uuid::Format::Bytes16);
}
}  // namespace hestia