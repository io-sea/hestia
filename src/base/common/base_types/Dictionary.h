#pragma once

#include "Metadata.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace hestia {

class Dictionary {
  public:
    using Ptr = std::unique_ptr<Dictionary>;

    enum class Type { SCALAR, SEQUENCE, MAP };

    Dictionary(Type type = Type::MAP);

    static Ptr create(Type type = Type::MAP);

    void add_sequence_item(std::unique_ptr<Dictionary> item);

    using onItem =
        std::function<void(const std::string& key, const std::string& value)>;
    void for_each_scalar(onItem func) const;

    Dictionary* get_map_item(const std::string& key) const;

    const std::string& get_scalar() const;

    Type get_type() const;

    const std::vector<std::unique_ptr<Dictionary>>& get_sequence() const;
    const std::unordered_map<std::string, std::unique_ptr<Dictionary>>&
    get_map() const;

    void get_map_items(
        Metadata& sink,
        const std::vector<std::string>& exclude_keys = {}) const;

    bool has_key_and_value(
        const Metadata::Query& query,
        const std::string& delimiter = "::") const;

    bool has_map_item(const std::string& key) const;

    void set_map_item(const std::string& key, std::unique_ptr<Dictionary> item);

    void set_scalar(const std::string& scalar);

    void set_map(const std::unordered_map<std::string, std::string>& items);

    bool has_tag() const;

    const std::pair<std::string, std::string>& get_tag() const;

    void set_tag(const std::string& tag, const std::string& prefix = "");

    bool is_empty() const;

    bool operator==(const Dictionary& rhs) const;  // Deep Comparision

  private:
    Type m_type{Type::MAP};
    std::pair<std::string, std::string> m_tag;
    std::string m_scalar;
    std::vector<std::unique_ptr<Dictionary>> m_sequence;
    std::unordered_map<std::string, std::unique_ptr<Dictionary>> m_map;
};

}  // namespace hestia