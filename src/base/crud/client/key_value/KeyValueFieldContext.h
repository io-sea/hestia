#pragma once

#include "StringAdapter.h"

namespace hestia {

class KeyValueFieldContext {
  public:
    KeyValueFieldContext(
        const AdapterCollection* adapters, const std::string& key_prefix);

    virtual ~KeyValueFieldContext() = default;

    std::string get_item_key(const std::string& id) const;

    std::string get_field_key(
        const std::string& field, const std::string& value) const;

    void get_item_keys(
        const std::vector<std::string>& ids,
        std::vector<std::string>& keys) const;

    std::string get_set_key() const;

    std::string get_prefix() const;

  protected:
    const AdapterCollection* m_adapters{nullptr};
    std::string m_key_prefix;
};

}  // namespace hestia