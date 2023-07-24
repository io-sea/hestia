#include "BlockStore.h"

#include <fstream>
#include <stdexcept>

namespace hestia {

BlockStore::BlockStore(const Uuid& id) : m_id(id) {}

const Uuid& BlockStore::id() const
{
    return m_id;
}

bool BlockStore::has_key(const std::string& key)
{
    return m_data.find(key) != m_data.end();
}

void BlockStore::remove(const std::string& key)
{
    if (auto iter = m_data.find(key); iter != m_data.end()) {
        m_data.erase(iter);
    }
}

const BlockList& BlockStore::get_block_list(const std::string& key) const
{
    if (auto iter = m_data.find(key); iter != m_data.end()) {
        return iter->second;
    }
    else {
        throw std::runtime_error("Requested blocklist not found");
    }
}

void BlockStore::set_block_list(const std::string& key, const BlockList& blocks)
{
    m_data[key] = blocks;
}

std::string BlockStore::dump() const
{
    std::string output;
    for (const auto& [key, blocklist] : m_data) {
        output += "Key: " + key + '\n';
        output += blocklist.dump() + '\n';
    }
    return output;
}

void BlockStore::load(const std::filesystem::path& directory)
{
    (void)directory;
}

BlockStore::ReturnCode BlockStore::write(
    const std::string& key,
    const Extent& extent,
    const ReadableBufferView& buffer)
{
    auto iter = m_data.find(key);
    if (iter == m_data.end()) {
        m_data[key] = BlockList();
        m_data[key].write(extent, buffer);
        return {};
    }
    iter->second.write(extent, buffer);
    return {};
}

BlockStore::ReturnCode BlockStore::read(
    const std::string& key,
    const Extent& extent,
    WriteableBufferView& buffer) const
{
    auto iter = m_data.find(key);
    if (iter == m_data.end()) {
        const auto msg = "Key " + key + " not found during read";
        return ReturnCode{ReturnCode::Status::ID_NOT_FOUND, msg};
    }

    const auto& [found, bytes_read] = iter->second.read(extent, buffer);
    if (!found) {
        const auto msg = "Read Failed to find extent: " + extent.to_string();
        return ReturnCode{ReturnCode::Status::EXTENT_NOT_FOUND, msg};
    }
    return BlockStore::ReturnCode{bytes_read};
}
}  // namespace hestia