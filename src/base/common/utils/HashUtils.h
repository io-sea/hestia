#pragma once

#include <string>

namespace hestia {
class HashUtils {
  public:
    static std::string do_sha256(const std::string& input);

    static std::string do_md5(const std::string& input);

    static std::string do_rand_32();

    static std::string do_h_mac(
        const std::string& key, const std::string& code);

    static std::string do_h_mac_hex(
        const std::string& key, const std::string& code);

    static std::string uri_encode(const std::string& input, bool encode_slash);

    static std::string base64_encode(const std::string& input);
};
}  // namespace hestia