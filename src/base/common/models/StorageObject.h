#pragma once

#include "Metadata.h"
#include "OwnableModel.h"

#include <ostream>

namespace hestia {

/**
 * @brief An 'Object' as might be kept in an Object Store
 *
 * The StorageObject class is a generic representation of an 'Object Store' data
 * storage Object.
 */
class StorageObject : public OwnableModel {
  public:
    /**
     * Constructor
     *
     * @param id A name for the object - can be path or UUID-like.
     */
    StorageObject(const std::string& name = {});

    StorageObject(const Uuid& id);

    void deserialize(
        const Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL) override;

    /**
     * Does the object have zero size
     * @return True if the object has zero size
     */
    bool empty() const;

    void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const override;

    /**
     * A string representation of the object - intended for debugging
     * @return A string representation of the object
     */
    std::string to_string() const;

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

    Metadata m_metadata;
    std::size_t m_size{0};
};
}  // namespace hestia