#pragma once

#include "Metadata.h"

#include <ctime>
#include <ostream>

namespace hestia {

/**
 * @brief An 'Object' as might be kept in an Object Store
 *
 * The StorageObject class is a generic representation of an 'Object Store' data
 * storage Object.
 */
class StorageObject {
  public:
    /**
     * Constructor
     *
     * @param id An id for the object - typically but not necessarily a UUID.
     */
    StorageObject(const std::string& id = {});

    /**
     * Does the object have zero size
     * @return True if the object has zero size
     */
    bool empty() const;

    /**
     * Return the Object's id - typically but not necessarily a UUID. It can be
     * empty.
     * @return The Object's id
     */
    const std::string& id() const;

    /**
     * Set the created and last-modified timestamps to current unix time
     */
    void initialize_timestamps();

    /**
     * A string representation of the object - intended for debugging
     * @return A string representation of the objec
     */
    std::string to_string() const;

    /**
     * Specify the last-modified time since unix epoch
     * @param time the last-modified time
     */
    void update_modified_time(std::time_t time);

    /**
     * Set the last-modified time to the current time
     */
    void update_modified_time();

    /**
     * True if the object has a non-empty id
     * @return True if the object has a non-empty id
     */
    bool valid() const;

    bool operator==(const StorageObject& other) const
    {
        return m_id == other.m_id && m_metadata == other.m_metadata;
    }

    friend std::ostream& operator<<(
        std::ostream& os, StorageObject const& value)
    {
        os << value.to_string();
        return os;
    }

    std::string m_id;
    Metadata m_metadata;
    std::size_t m_size{0};
    std::time_t m_creation_time{0};
    std::time_t m_last_modified_time{0};
};
}  // namespace hestia