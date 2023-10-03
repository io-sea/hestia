#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace hestia {

/**
 * @brief A Map is a collection of string-typed Key-Value pairs.
 *
 * This class wraps an STL unordered_map and provides convenience methods,
 * semantically it is a collection of string-typed Key-Value pairs.
 */

using KeyValuePair    = std::pair<std::string, std::string>;
using VecKeyValuePair = std::vector<KeyValuePair>;

class Map {
  public:
    virtual ~Map() = default;

    /**
     * Return the underlying STL container - this is mainly intended for
     * bulk copying rather than direct data access.
     * @return the underlying STL container
     */
    const std::unordered_map<std::string, std::string>& data() const;

    void add_key_prefix(const std::string& prefix);

    void copy_with_prefix(
        const std::vector<std::string>& prefixes,
        Map& copy_to,
        const std::string& replacement_prefix = {},
        bool remove_prefix                    = true) const;

    /**
     * Return true if there are no key-value entries
     * @return true if there are no key-value entries
     */
    bool empty() const;

    using onItem =
        std::function<void(const std::string& key, const std::string& value)>;
    /**
     * Call func for each value in the container.
     * @param func the function to call with each value
     */
    void for_each_item(onItem func) const;

    /**
     * Return the item with this key
     * @param key the key for the map
     * @return the value or an empty string if not found
     */
    std::string get_item(const std::string& key) const;

    bool has_item(const std::string& key) const;

    /**
     * Return whether the Query (key-value pair) is contained in the map
     * @param query A key-value pair for searching in the container
     * @return true if the key is found and its value matches the supplied one
     */
    bool has_key_and_value(const KeyValuePair& query) const;

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
    void merge(const Map& other);

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
    std::string to_string(bool sort_keys = false) const;

    bool operator==(const Map& other) const { return m_data == other.m_data; }

    friend std::ostream& operator<<(std::ostream& os, Map const& value)
    {
        os << value.to_string();
        return os;
    }

  private:
    std::unordered_map<std::string, std::string> m_data;
};

}  // namespace hestia