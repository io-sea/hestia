#include "UserTokenGenerator.h"

#include "HashUtils.h"

namespace hestia {

UserTokenGenerator::~UserTokenGenerator() {}
std::string UserTokenGenerator::generate(const std::string&) const
{
    return HashUtils::base64_encode(HashUtils::do_rand_32());
}

}  // namespace hestia