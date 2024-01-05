#pragma once

#include "Dictionary.h"
#include "Uuid.h"

namespace hestia {

/**
 * @brief Base class for Items that can be converted to and from a Dictionary
 *
 * Items can have an optional 'type' and 'identifier'
 *
 * The Format specifier is a hint indicating:
 * - ID: just an 'identifier' field is included
 * - NO_CHILD: Don't include any chilren - even child ids
 * - CHILD_ID: children are only represented by an identifier
 * - MODIFIED: Only modified fields are included
 * - FULL: All fields are included
 */

class Serializeable {
  public:
    enum class Format { UNSET, ID, CHILD_ID, MODIFIED, FULL };

    Serializeable(const std::string& type = {}) : m_type(type) {}

    virtual ~Serializeable() = default;

    virtual std::string get_runtime_type() const { return m_type; }

    virtual void deserialize(
        const Dictionary& dict, Format format = Format::FULL) = 0;

    virtual void serialize(
        Dictionary& dict, Format format = Format::FULL) const = 0;

  protected:
    std::string m_type;
};
}  // namespace hestia