#pragma once

#include "Index.h"
#include "Map.h"
#include "SearchExpression.h"

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
    enum class ScalarType { INT, FLOAT, STRING, BOOL, NULL_T };

    struct FormatSpec {
        FormatSpec() : m_sequence_delimiter("\n\n") {}

        std::string m_sequence_delimiter{"\n\n"};
        std::string m_scope_delimiter{"."};
        Map::FormatSpec m_map_format;
    };

    Dictionary(const Map& flat, const FormatSpec& format);

    Dictionary(const std::string& serialized, const FormatSpec& format);

    Dictionary(Type type = Type::MAP);

    Dictionary(const Dictionary& other);

    static Ptr create(const Dictionary& other);

    static Ptr create(Type type = Type::MAP);

    static Ptr create(const Map& flat, const FormatSpec& format);

    void add_scalar_item(
        const std::string& key,
        const std::string& value,
        ScalarType scalar_type);

    void add_sequence_item(Dictionary::Ptr item);

    void expand(const Map& flat_representation, const FormatSpec& format = {});

    void flatten(Map& flat_representation, const FormatSpec& format = {}) const;

    using onItem =
        std::function<void(const std::string& key, const std::string& value)>;
    void for_each_scalar(onItem func) const;

    Dictionary* get_map_item(const std::string& key) const;

    const Dictionary* get_item(std::size_t idx) const;

    Dictionary::Ptr get_copy_of_item(std::size_t idx) const;

    const std::string& get_scalar() const;

    std::string get_scalar(
        const std::string& key, const std::string& delimiter = "::") const;

    ScalarType get_scalar_type() const;

    std::size_t get_size() const;

    void get_scalars(
        const std::string& key, std::vector<std::string>& values) const;

    void get_values(
        const SearchExpression& search, std::vector<std::string>& values) const;

    const std::pair<std::string, std::string>& get_tag() const;

    Type get_type() const;

    const std::vector<Dictionary::Ptr>& get_sequence() const;

    const std::unordered_map<std::string, Dictionary::Ptr>& get_map() const;

    void get_map_items(
        Map& sink, const std::vector<std::string>& exclude_keys = {}) const;

    void merge(const Dictionary& dict, bool append = false);

    bool has_key_and_value(
        const KeyValuePair& kv_pair, const std::string& delimiter = "::") const;

    bool has_map_item(const std::string& key) const;

    bool has_tag() const;

    bool is_empty() const;

    bool is_sequence() const;

    bool is_scalar() const;

    bool is_map() const;

    Dictionary* set_map_item(
        const std::string& key, Dictionary::Ptr item = nullptr);

    void set_map_scalar(
        const std::string& key,
        const std::string& value,
        ScalarType scalar_type,
        const std::string& tag = {});

    /**
     * Set the node to be of scalar type and its value
     *
     * The class is agnostic to the scalar type, but does take a 'should_quote'
     * hint for conversion to other forms, for example yaml.
     * @param scalar The value of the scalar
     */
    void set_scalar(const std::string& scalar, ScalarType scalar_type);

    void set_map(const std::unordered_map<std::string, std::string>& items);

    void set_sequence(const VecKeyValuePair& items);

    void set_tag(const std::string& tag, const std::string& prefix = "");

    void set_type(Type type);

    void from_string(
        const std::string& buffer,
        const std::string& key_prefix = {},
        const FormatSpec& format      = {});

    std::string to_string(const FormatSpec& format = {}) const;

    void write(
        std::string& buffer,
        const Index& index       = {},
        const FormatSpec& format = {}) const;

    bool operator==(const Dictionary& rhs) const;

    Dictionary& operator=(const Dictionary& other);

  private:
    void copy_from(const Dictionary& other);

    Type m_type{Type::MAP};
    std::pair<std::string, std::string> m_tag;
    std::string m_scalar;
    ScalarType m_scalar_type{ScalarType::STRING};
    std::vector<Dictionary::Ptr> m_sequence;
    std::unordered_map<std::string, Dictionary::Ptr> m_map;
};

}  // namespace hestia