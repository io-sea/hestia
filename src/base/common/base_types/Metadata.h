#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace hestia {

/**
 * @brief A map-like class with convenience methods for handling key-value entries
 *
 * The Metadata class is a key-value map with some extra convenience methods.
 */
class Metadata {
  public:
    virtual ~Metadata() = default;

    using onItem =
        std::function<void(const std::string& key, const std::string& value)>;

    /**
     * Call func for each value in the container - if a key_prefix is specified
     * only operate on values corresponding to keys with this prefix.
     * @param func the funciton to call with each value
     * @param key_prefix optional prefix for keys - if a key starts with it then call func for its value
     */
    void for_each_item(onItem func, const std::string& key_prefix = {}) const;

    /**
     * Return the item with this key
     * @param key the key for the map
     * @return the value or an empty string if not found
     */
    std::string get_item(const std::string& key) const;

    using Query = std::pair<std::string, std::string>;

    /**
     * Return whether the Query (key-value pair) is contained in the map
     * @param query A key-value pair for searching in the container
     * @return true if the key is found and its value matches the supplied one
     */
    bool has_key_and_value(const Query& query) const;

    /**
     * Does the value corresponding to the input key contain the search_term?
     * @param key A key into the container
     * @param search_term This value will be checked against they container value corresponding to the input key for inclusion
     * @return true if the container has the key and its value includes the search_term
     */
    bool key_contains(
        const std::string& key, const std::string& search_term) const;

    /**
     * Merge this container with the input one - existing key values are
     * overwritten if duplicated
     * @param other The other container to merge in
     */
    void merge(const Metadata& other);

    /**
     * Set the value corresponding to the key to item
     * @param key Container (map) key
     * @param item Container value for this key
     */
    void set_item(const std::string& key, const std::string& item);

    /**
     * A string representation of the container - intended for debugging
     * @return A string representation of the container
     */
    std::string to_string() const;

    bool operator==(const Metadata& other) const
    {
        return m_data == other.m_data;
    }

    friend std::ostream& operator<<(std::ostream& os, Metadata const& value)
    {
        os << value.to_string();
        return os;
    }

  private:
    std::unordered_map<std::string, std::string> m_data;
};

class NestedMetadata : public Metadata {
  public:
    void add_child(
        const std::string& key, std::unique_ptr<NestedMetadata> child);

    NestedMetadata* get_child(const std::string& key) const;

    bool has_child(const std::string& key) const;

  private:
    std::unordered_map<std::string, std::unique_ptr<NestedMetadata>> m_children;
};

}  // namespace hestia