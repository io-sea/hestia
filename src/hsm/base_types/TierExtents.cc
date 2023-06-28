#include "TierExtents.h"

namespace hestia {

TierExtents::TierExtents(uint8_t tier_id, const Extent& extent) :
    m_tier_id(tier_id)
{
    m_extents[extent.m_offset] = extent;
}

void TierExtents::add_extent(const Extent& extent)
{
    m_extents[extent.m_offset] = extent;
}

void TierExtents::remove_extent(const Extent& extent)
{
    if (m_extents.size() == 1) {
        m_extents.erase(extent.m_offset);
    }
}

bool TierExtents::empty() const
{
    return m_extents.empty();
}

std::size_t TierExtents::get_size() const
{
    if (m_extents.empty()) {
        return 0;
    }
    return m_extents.rbegin()->second.get_end();
}

void TierExtents::read_lock()
{
    m_read_lock.lock();
}

void TierExtents::write_lock()
{
    m_write_lock.lock();
}

void TierExtents::read_unlock()
{
    m_read_lock.unlock();
}

void TierExtents::write_unlock()
{
    m_write_lock.unlock();
}

bool TierExtents::is_read_locked()
{
    return m_read_lock.m_active;
}

bool TierExtents::is_write_locked()
{
    return m_write_lock.m_active;
}

Dictionary::Ptr TierExtents::serialize() const
{
    auto dict = std::make_unique<Dictionary>();

    std::unordered_map<std::string, std::string> data;
    data["tier"] = std::to_string(m_tier_id);
    dict->set_map(data);

    dict->set_map_item("read_lock", m_read_lock.serialize());
    dict->set_map_item("write_lock", m_write_lock.serialize());

    auto extent_seq_dict =
        std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& extent : m_extents) {
        auto extent_dict = std::make_unique<Dictionary>();
        std::unordered_map<std::string, std::string> extent_data = {
            {"offset", std::to_string(extent.second.m_offset)},
            {"length", std::to_string(extent.second.m_length)},
        };
        extent_dict->set_map(extent_data);
        extent_seq_dict->add_sequence_item(std::move(extent_dict));
    }
    dict->set_map_item("extents", std::move(extent_seq_dict));
    return dict;
}

void TierExtents::deserialize(const Dictionary& dict)
{
    for (const auto& [key, dict_item] : dict.get_map()) {
        if (key == "tier") {
            const auto value = dict_item->get_scalar();
            if (!value.empty()) {
                m_tier_id = static_cast<uint8_t>(std::stoi(value));
            }
        }
        else if (key == "read_lock") {
            m_read_lock.deserialize(*dict_item);
        }
        else if (key == "write_lock") {
            m_write_lock.deserialize(*dict_item);
        }
        else if (key == "extents") {
            if (dict_item->get_type() == Dictionary::Type::SEQUENCE) {

                for (const auto& extent_dict : dict_item->get_sequence()) {
                    std::size_t offset{0};
                    std::size_t length{0};

                    if (auto offset_dict = extent_dict->get_map_item("offset");
                        offset_dict != nullptr) {
                        if (!offset_dict->get_scalar().empty()) {
                            offset = std::stoul(offset_dict->get_scalar());
                        }
                    }
                    if (auto length_dict = extent_dict->get_map_item("length");
                        length_dict != nullptr) {
                        if (!length_dict->get_scalar().empty()) {
                            length = std::stoul(length_dict->get_scalar());
                        }
                    }
                    m_extents.emplace(offset, Extent{offset, length});
                }
            }
        }
    }
}

}  // namespace hestia