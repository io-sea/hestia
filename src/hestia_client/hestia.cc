#include "hestia_old.h"

#include "HestiaCppInterface.h"

#include <fstream>

namespace hestia::old {

int put(
    const struct hsm_uint oid,
    const bool is_overwrite,
    const void* buf,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier)
{
    return HestiaCppInterface::put(
        ostk::Uuid(oid.lower, oid.higher), ostk::Extent(offset, length),
        ostk::ReadableBufferView(buf, length), target_tier, is_overwrite);
}

int put(
    const struct hsm_uint oid,
    const bool is_overwrite,
    std::ifstream& infile,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier)
{
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
    ostk::WriteableBufferView writeable_buffer(buf, length);
    return HestiaCppInterface::get(
        ostk::Uuid(oid.lower, oid.higher), ostk::Extent(offset, length),
        writeable_buffer, src_tier);
}

int set_attrs(const struct hsm_uint& oid, const char* attrs)
{
    return HestiaCppInterface::set_attributes(
        ostk::Uuid(oid.lower, oid.higher), attrs);
}

std::string get_attrs(const struct hsm_uint& oid, const char* attr_keys)
{
    std::string result;
    HestiaCppInterface::get_attributes(
        ostk::Uuid(oid.lower, oid.higher), attr_keys, result);
    return result;
}

std::string list_attrs(const struct hsm_uint& oid)
{
    std::string result;
    HestiaCppInterface::list_attributes(
        ostk::Uuid(oid.lower, oid.higher), result);
    return result;
}

int remove(const struct hsm_uint& oid)
{
    return HestiaCppInterface::release(ostk::Uuid(oid.lower, oid.higher));
}

int release(const struct hsm_uint& oid, const std::uint8_t src_tier)
{
    return HestiaCppInterface::release(
        ostk::Uuid(oid.lower, oid.higher), src_tier);
}

std::vector<struct hsm_uint> list(const std::uint8_t tier)
{
    std::vector<ostk::Uuid> object_ids;
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
    return HestiaCppInterface::copy(
        ostk::Uuid(oid.lower, oid.higher), {0, 0}, src_tier, tgt_tier);
}

int move(
    const struct hsm_uint& oid,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier)
{
    return HestiaCppInterface::move(
        ostk::Uuid(oid.lower, oid.higher), {0, 0}, src_tier, tgt_tier);
}

std::vector<uint8_t> locate(const struct hsm_uint& oid)
{
    std::vector<uint8_t> tiers;
    HestiaCppInterface::list_tiers(ostk::Uuid(oid.lower, oid.higher), tiers);
    return tiers;
}
}  // namespace hestia::old