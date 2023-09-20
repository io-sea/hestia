#pragma once

#include "Map.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace hestia {

/**
 * @brief Container type for nested Key-Value data
 *
 * The Dictionary type is intended to be a simple intermediate format between
 * Objects and serialization targets like JSON or YAML.
 *
 * We convert Objects first to Dictionaries, pass them around and if needed into
 * their final format(s).
 *
 * Dictionaries support a subset of JSON, they are comprised of:
 * - SCALAR: a single string-type item
 * - SEQUENCE: a linear container of Dictionaries
 * - MAP: Key-Value pairs, with string-type Keys and Dictionary-type Values
 *
 * This class also supports 'expand' and 'flatten' operations, where the
 * heirarchical Dictionary structure is convered into a flat Map of string-type
 * Key-Value pairs. The heirarchy is maintained by namespacing the keys with a
 * chosen delimiter.
 */

class Dictionary {
  public:
    using Ptr = std::unique_ptr<Dictionary>;

    enum class Type { SCALAR, SEQUENCE, MAP };

    Dictionary(Type type = Type::MAP);

    Dictionary(const Dictionary& other);

    static Ptr create(Type type = Type::MAP);

    void add_sequence_item(std::unique_ptr<Dictionary> item);

    void expand(const Map& flat_representation);

    void flatten(Map& flat_representation) const;

    using onItem =
        std::function<void(const std::string& key, const std::string& value)>;
    void for_each_scalar(onItem func) const;

    Dictionary* get_map_item(const std::string& key) const;

    const std::string& get_scalar() const;

    std::string get_scalar(
        const std::string& key, const std::string& delimiter = "::") const;

    void get_scalars(
        const std::string& key, std::vector<std::string>& values) const;

    const std::pair<std::string, std::string>& get_tag() const;

    Type get_type() const;

    const std::vector<std::unique_ptr<Dictionary>>& get_sequence() const;

    const std::unordered_map<std::string, std::unique_ptr<Dictionary>>&
    get_map() const;

    void get_map_items(
        Map& sink, const std::vector<std::string>& exclude_keys = {}) const;

    void merge(const Dictionary& dict);

    bool has_key_and_value(
        const KeyValuePair& kv_pair, const std::string& delimiter = "::") const;

    bool has_map_item(const std::string& key) const;

    bool has_tag() const;

    bool is_empty() const;

    void set_map_item(const std::string& key, std::unique_ptr<Dictionary> item);

    /**
     * Set the node to be of scalar type and its value
     *
     * The class is agnostic to the scalar type, but does take a 'should_quote'
     * hint for conversion to other forms, for example yaml.
     * @param scalar The value of the scalar
     * @param should_quote A hint for converts on whether this value should be quoted, e.g. as "my_scalar_value".
     */
    void set_scalar(const std::string& scalar, bool should_quote = false);

    /**
     * Return true if this scalar type should be quoted - useful for string
     * types with possible special characters
     *
     * @return true if this scalar type should be quoted
     */
    bool should_quote_scalar() const;

    void set_map(const std::unordered_map<std::string, std::string>& items);

    void set_tag(const std::string& tag, const std::string& prefix = "");

    void set_type(Type type);

    std::string to_string(bool sort_keys = false) const;  // For debugging

    bool operator==(const Dictionary& rhs) const;  // Deep Comparision

    Dictionary& operator=(const Dictionary& other);

  private:
    void copy_from(const Dictionary& other);

    const std::string m_delim = ".";
    Type m_type{Type::MAP};
    std::pair<std::string, std::string> m_tag;
    std::string m_scalar;
    bool m_should_quote{false};
    std::vector<std::unique_ptr<Dictionary>> m_sequence;
    std::unordered_map<std::string, std::unique_ptr<Dictionary>> m_map;
};

}  // namespace hestia