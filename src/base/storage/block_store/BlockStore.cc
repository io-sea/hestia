#include "BlockStore.h"

#include <fstream>

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

void BlockStore::dump(const std::filesystem::path& directory)
{
    if (!std::filesystem::is_directory(directory)) {
        std::filesystem::create_directories(directory);
    }

    for (const auto& [key, blocklist] : m_data) {
        std::filesystem::path filename = key + ".dat";
        std::ofstream out;
        out.open(directory / filename);
        out << blocklist.dump();
    }
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