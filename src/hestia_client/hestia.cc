#include "hestia.h"

#include "HestiaConfigurator.h"
#include "HestiaCppInterface.h"
#include "Logger.h"

#include <fstream>

namespace hestia {

bool g_intialized{false};

void initialize(const char* config_path)
{
    HestiaConfig config;
    if (config_path != nullptr) {
        config.load(config_path);
    }
    else {
        config.load_defaults();
    }

    hestia::HestiaConfigurator configurator;
    const auto result = configurator.initialize(config);
    if (!result.ok()) {
        LOG_ERROR("Error configuring Hestia: " + result.message());
    }
    g_intialized = true;
}

int put(
    const struct hsm_uint oid,
    const bool is_overwrite,
    const void* buf,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier)
{
    if (!g_intialized) {
        initialize();
    }

    return HestiaCppInterface::put(
        hestia::Uuid(oid.lower, oid.higher), hestia::Extent(offset, length),
        hestia::ReadableBufferView(buf, length), target_tier, is_overwrite);
}

int put(
    const struct hsm_uint oid,
    const bool is_overwrite,
    std::ifstream& infile,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier)
{
    if (!g_intialized) {
        initialize();
    }

    std::string buf;
    buf.resize(length);
    if (!infile.is_open()) {
        return -1;
    }
    infile.seekg(offset, infile.beg);
    infile.read(&buf[0], length);
    return put(oid, is_overwrite, buf.data(), offset, length, target_tier);
}

int get(
    const struct hsm_uint oid,
    void* buf,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t src_tier,
    const std::uint8_t)
{
    if (!g_intialized) {
        initialize();
    }

    hestia::WriteableBufferView writeable_buffer(buf, length);
    return HestiaCppInterface::get(
        hestia::Uuid(oid.lower, oid.higher), hestia::Extent(offset, length),
        writeable_buffer, src_tier);
}

int set_attrs(const struct hsm_uint& oid, const char* attrs)
{
    if (!g_intialized) {
        initialize();
    }

    return HestiaCppInterface::set_attributes(
        hestia::Uuid(oid.lower, oid.higher), attrs);
}

std::string get_attrs(const struct hsm_uint& oid, const char* attr_keys)
{
    if (!g_intialized) {
        initialize();
    }

    std::string result;
    HestiaCppInterface::get_attributes(
        hestia::Uuid(oid.lower, oid.higher), attr_keys, result);
    return result;
}

std::string list_attrs(const struct hsm_uint& oid)
{
    if (!g_intialized) {
        initialize();
    }

    std::string result;
    HestiaCppInterface::list_attributes(
        hestia::Uuid(oid.lower, oid.higher), result);
    return result;
}

int remove(const struct hsm_uint& oid)
{
    if (!g_intialized) {
        initialize();
    }

    return HestiaCppInterface::release(hestia::Uuid(oid.lower, oid.higher));
}

int release(const struct hsm_uint& oid, const std::uint8_t src_tier)
{
    if (!g_intialized) {
        initialize();
    }

    return HestiaCppInterface::release(
        hestia::Uuid(oid.lower, oid.higher), src_tier);
}

std::vector<struct hsm_uint> list(const std::uint8_t tier)
{
    if (!g_intialized) {
        initialize();
    }

    std::vector<hestia::Uuid> object_ids;
    HestiaCppInterface::list_objects(tier, object_ids);

    std::vector<hsm_uint> hestia_ids;
    for (const auto& id : object_ids) {
        hestia_ids.push_back({id.m_lo, id.m_hi});
    }
    return hestia_ids;
}

int copy(
    const struct hsm_uint& oid,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier)
{
    if (!g_intialized) {
        initialize();
    }

    return HestiaCppInterface::copy(
        hestia::Uuid(oid.lower, oid.higher), {0, 0}, src_tier, tgt_tier);
}

int move(
    const struct hsm_uint& oid,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier)
{
    if (!g_intialized) {
        initialize();
    }

    return HestiaCppInterface::move(
        hestia::Uuid(oid.lower, oid.higher), {0, 0}, src_tier, tgt_tier);
}

std::vector<uint8_t> locate(const struct hsm_uint& oid)
{
    if (!g_intialized) {
        initialize();
    }

    std::vector<uint8_t> tiers;
    HestiaCppInterface::list_tiers(hestia::Uuid(oid.lower, oid.higher), tiers);
    return tiers;
}
}  // namespace hestia