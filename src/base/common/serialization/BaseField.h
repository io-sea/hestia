#pragma once

#include <string>

namespace hestia {

/**
 * @brief A named attribute of an Object with modification monitoring
 *
 * A Field is a named attribute of an Object. It allows for simple
 * reflection-like functionality used in Serialization by referencing the field
 * 'name'.
 *
 * In addition the class adds the notion of the attribute being 'modified',
 * which is useful for CRUD-like functionality.
 */
class BaseField {
  public:
    BaseField(const std::string& name, bool index_on = false);

    virtual ~BaseField() = default;

    const std::string& get_name() const;

    bool is_primary_key() const;

    bool modified() const;

    void reset();

    void set_index_on(bool index_on);

    void set_is_primary_key(bool is_primary);

    bool should_index_on() const;

  protected:
    bool m_modified{false};
    bool m_index_on{false};
    bool m_is_primary_key{false};
    std::string m_name;
};

}  // namespace hestia