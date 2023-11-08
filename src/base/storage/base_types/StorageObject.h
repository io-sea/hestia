#pragma once

#include "Map.h"
#include "SerializeableWithFields.h"

#include <ostream>

namespace hestia {

/**
 * @brief An 'Object' as might be kept in an Object Store
 *
 * The StorageObject class is a generic representation of an 'Object Store' data
 * storage Object.
 */
class StorageObject : public SerializeableWithFields {
  public:
    /**
     * Constructor
     *
     * @param id A name for the object - can be path or UUID-like.
     */
    StorageObject(const std::string& id = {});

    StorageObject(const StorageObject& other);

    /**
     * Does the object have zero size
     * @return True if the object has zero size
     */
    bool empty() const;

    Map& get_metadata_as_writeable();

    void merge_metadata(const Map& other)
    {
        get_metadata_as_writeable().merge(other);
    }

    const Map& metadata() const;

    const std::string& get_location() const;

    std::string get_metadata_item(const std::string& key) const;

    void set_metadata(const std::string& key, const std::string& value);

    std::size_t size() const { return m_size.get_value(); }

    void set_size(std::size_t size) { m_size.update_value(size); }

    void set_location(const std::string& address);

    StorageObject& operator=(const StorageObject& other);

    /**
     * A string representation of the object - intended for debugging
     * @return A string representation of the object
     */
    std::string to_string() const;

    friend std::ostream& operator<<(
        std::ostream& os, StorageObject const& value)
    {
        os << value.to_string();
        return os;
    }

  private:
    void init();
    static constexpr const char s_type[]{"storage_object"};

    ScalarMapField m_metadata{"metadata"};
    StringField m_location{"location"};
    UIntegerField m_size{"size"};
};
}  // namespace hestia