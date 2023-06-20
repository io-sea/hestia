#include "HashUtils.h"

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "Logger.h"

namespace hestia {

std::string HashUtils::do_rand_32()
{
    std::vector<unsigned char> buffer(32);
    int rc = RAND_bytes(buffer.data(), buffer.size());
    if (rc != 1) {
        throw std::runtime_error(
            "SSL Error with code: " + std::to_string(ERR_get_error()));
    }
    return {buffer.begin(), buffer.end()};
}

std::string HashUtils::base64_encode(const std::string& input)
{
    std::vector<unsigned char> buffer(EVP_MAX_MD_SIZE);
    EVP_EncodeBlock(
        buffer.data(), reinterpret_cast<const unsigned char*>(input.c_str()),
        input.length());

    std::string ret(buffer.begin(), buffer.end());
    ret.erase(std::find(ret.begin(), ret.end(), '\0'), ret.end());

    return ret;
}

std::string HashUtils::uri_encode(const std::string& input, bool encode_slash)
{
    std::stringstream sstr;
    for (const auto c : input) {
        if ((std::isalnum(c) != 0) || c == '_' || c == '-' || c == '~'
            || c == '.') {
            sstr << c;
        }
        else if (c == '/') {
            if (encode_slash) {
                sstr << "%2F";
            }
            else {
                sstr << c;
            }
        }
        else if (c == ' ') {
            sstr << "%20";
        }
        else {
            sstr << '%' << std::hex << std::uppercase << std::setw(2)
                 << std::setfill('0') << int(c);
            sstr << std::dec;
        }
    }
    return sstr.str();
}

std::string HashUtils::do_sha256(const std::string& input)
{
    unsigned char* hash = new unsigned char[EVP_MAX_MD_SIZE];
    unsigned int result_size{0};

    int result = EVP_Digest(
        &input[0], input.size(), hash, &result_size, EVP_sha256(), nullptr);
    if (result != 1) {
        delete[] hash;
        return "";
    }

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < result_size; i++) {
        ss << std::hex << std::setw(2) << (unsigned int)hash[i];
    }
    delete[] hash;
    return ss.str();
}

std::string HashUtils::do_h_mac(const std::string& key, const std::string& msg)
{
    unsigned int result_size{0};
    const auto hash = HMAC(
        EVP_sha256(), &key[0], key.length(), (unsigned char*)&msg[0],
        msg.length(), nullptr, &result_size);

    std::stringstream ss;
    ss << std::setfill('0');
    for (unsigned int i = 0; i < result_size; i++) {
        ss << hash[i];
    }
    return ss.str();
}

std::string HashUtils::do_h_mac_hex(
    const std::string& key, const std::string& msg)
{
    unsigned int result_size{0};
    const auto hash = HMAC(
        EVP_sha256(), &key[0], key.length(), (unsigned char*)&msg[0],
        msg.length(), NULL, &result_size);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < result_size; i++) {
        ss << std::hex << std::setw(2) << (unsigned int)hash[i];
    }
    return ss.str();
}
}  // namespace hestia