#include "Extent.h"

#include <algorithm>
#include <sstream>

namespace hestia {

Extent::Extent(std::size_t offset, std::size_t length) :
    m_offset(offset), m_length(length)
{
}

bool Extent::empty() const
{
    return m_length == 0;
}

std::size_t Extent::get_end() const
{
    return m_offset + m_length;
}

bool Extent::has_same_offset(const Extent& ext) const
{
    return m_offset == ext.m_offset;
}

bool Extent::ends_after(const Extent& ext) const
{
    return ext.get_end() < get_end();
}

static const Extent ext_fullrange = {0, 0xffffffffffffffff};

Extent Extent::get_full_range_extent()
{
    return ext_fullrange;
}

Extent Extent::get_right_split(const Extent& ext) const
{
    return {ext.get_end(), m_length - ext.m_length};
}

Extent Extent::get_left_split(const Extent& ext) const
{
    return {m_offset, ext.m_offset - m_offset};
}

Extent Extent::get_right_remainder(const Extent& ext) const
{
    return {ext.get_end(), get_end() - ext.get_end()};
}

bool Extent::joined_to_start_of(const Extent& ext) const
{
    return get_end() == ext.m_offset;
}

std::size_t Extent::get_overlapping_length(const Extent& ext) const
{
    return get_left_most_end(*this, ext) - get_right_most_offset(*this, ext);
}

std::size_t Extent::get_right_most_offset(
    const Extent& ext0, const Extent& ext1)
{
    return std::max(ext0.m_offset, ext1.m_offset);
}

std::size_t Extent::get_left_most_offset(const Extent& ext0, const Extent& ext1)
{
    return std::min(ext0.m_offset, ext1.m_offset);
}

std::size_t Extent::get_right_most_end(const Extent& ext0, const Extent& ext1)
{
    return std::max(ext0.get_end(), ext1.get_end());
}

std::size_t Extent::get_left_most_end(const Extent& ext0, const Extent& ext1)
{
    return std::min(ext0.get_end(), ext1.get_end());
}

bool Extent::includes_or_overlaps(const Extent& ext) const
{
    if (empty() || ext.empty()) {
        return false;
    }
    return includes(ext) || overlaps(ext);
}

bool Extent::overlaps(const Extent& ext) const
{
    bool offset_in_extent =
        (m_offset >= ext.m_offset && m_offset <= ext.get_end() - 1);
    bool end_in_extent =
        (get_end() - 1 >= ext.m_offset && get_end() <= ext.get_end());
    return offset_in_extent || end_in_extent;
}

bool Extent::includes(const Extent& ext) const
{
    bool outside = (ext.m_offset < m_offset) || ext.get_end() > get_end();
    return !outside;
}

void Extent::serialize(Dictionary& dict, Format) const
{
    std::unordered_map<std::string, std::string> data = {
        {"offset", std::to_string(m_offset)},
        {"length", std::to_string(m_length)},
    };
    dict.add_scalar_item(
        "offset", std::to_string(m_offset), Dictionary::ScalarType::INT);
    dict.add_scalar_item(
        "length", std::to_string(m_length), Dictionary::ScalarType::INT);
}

void Extent::deserialize(const Dictionary& dict, Format)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "offset" && !value.empty()) {
            m_offset = std::stoul(value);
        }
        else if (key == "length" && !value.empty()) {
            m_length = std::stoull(value);
        }
    };
    dict.for_each_scalar(on_item);
}

std::string Extent::to_string() const
{
    std::stringstream sstr;
    sstr << "[" << m_offset << "->" << get_end() - 1 << "]";
    return sstr.str();
}

}  // namespace hestia