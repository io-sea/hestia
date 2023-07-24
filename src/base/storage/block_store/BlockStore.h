#pragma once

#include "BlockList.h"
#include "Uuid.h"
#include "WriteableBufferView.h"

#include <filesystem>
#include <unordered_map>

namespace hestia {

/**
 * @brief An in-memory Object Store primitive - it uses BlockList containers for storage
 *
 * This is a basic in-memory Object Store supporting Extents via BlockList
 * containers. It is intended mostly for development/mocking.
 */
class BlockStore {
  public:
    struct ReturnCode {
        enum class Status { ID_NOT_FOUND, EXTENT_NOT_FOUND, OK };

        ReturnCode() = default;

        ReturnCode(Status status, const std::string& message) :
            m_status(status), m_message(message)
        {
        }

        ReturnCode(std::size_t bytes_read) : m_bytes_read(bytes_read) {}

        bool is_ok() const { return m_status == Status::OK; }

        Status m_status{Status::OK};
        std::string m_message;
        std::size_t m_bytes_read{0};
    };

    /**
     * Constructor
     *
     * @param id optinal id in case of using multiple stores
     */
    BlockStore(const Uuid& id = {});

    /**
     * Dump the store to the filesystem at the given path - intended for
     * debugging
     *
     * @param directory Path to write the store to
     */
    std::string dump() const;

    /**
     * Return whether an object with this key has been added to the store
     *
     * @param key Object key to check
     * @return whether an object with this key has been added to the store
     */
    bool has_key(const std::string& key);

    /**
     * Return the store id
     *
     * @return the store id
     */
    const Uuid& id() const;

    const BlockList& get_block_list(const std::string& key) const;

    void set_block_list(const std::string& key, const BlockList& blocks);

    /**
     * Load the store into memory from the filesystem
     *
     * @param directory the location of the store
     */
    void load(const std::filesystem::path& directory);

    /**
     * Read an object from the store using the specified extent
     *
     * @param key key of the object to read
     * @param extent the extent (portion of) the object to read
     * @param buffer the buffer to read into
     * @return whether the operation was a sucess
     */
    ReturnCode read(
        const std::string& key,
        const Extent& extent,
        WriteableBufferView& buffer) const;

    /**
     * Remove the object from the store
     *
     * @param key key of the object to remove
     */
    void remove(const std::string& key);

    /**
     * Write an object to the store using the specified extent
     *
     * @param key key of the object to write
     * @param extent the extent (portion of) the object to write
     * @param buffer the buffer to read from
     * @return whether the operation was a sucess
     */
    ReturnCode write(
        const std::string& key,
        const Extent& extent,
        const ReadableBufferView& buffer);

  private:
    Uuid m_id;
    std::unordered_map<std::string, BlockList> m_data;
};
}  // namespace hestia