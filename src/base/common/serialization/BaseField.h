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
    enum class IndexScope { NONE, GLOBAL, PARENT };

    BaseField(
        const std::string& name, IndexScope index_scope = IndexScope::NONE);

    virtual ~BaseField() = default;

    const std::string& get_name() const;

    IndexScope get_index_scope() const;

    bool is_primary_key() const;

    bool modified() const;

    void reset();

    void set_index_scope(IndexScope index_scope);

    void set_is_primary_key(bool is_primary);

  protected:
    bool m_modified{false};
    IndexScope m_index_scope{IndexScope::NONE};
    bool m_is_primary_key{false};
    std::string m_name;
};

}  // namespace hestia