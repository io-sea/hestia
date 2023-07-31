#include "IdGenerator.h"

#include "HashUtils.h"
#include "SystemUtils.h"
#include "TimeUtils.h"
#include "UuidUtils.h"

#include <cassert>
#include <stdexcept>

#include <iostream>

namespace hestia {

DefaultIdGenerator::DefaultIdGenerator(uint64_t minimum_id) :
    m_minimum_id(minimum_id)
{
}

std::string DefaultIdGenerator::get_id(const std::string& key)
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

    std::vector<unsigned char> buffer;
    HashUtils::do_md5(
        key + std::to_string(TimeUtils::get_current_time()) + address_key,
        buffer);

    auto uuid = UuidUtils::from_bytes(buffer);
    if (m_minimum_id > 0) {
        uuid.bump_lower(m_minimum_id);
    }
    return UuidUtils::from_bytes(buffer).to_string();
}
}  // namespace hestia