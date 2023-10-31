#pragma once

#include "Dictionary.h"
#include "EnumUtils.h"

#include <string>
#include <vector>

namespace hestia {

class CrudIdentifier;
using VecCrudIdentifier = std::vector<CrudIdentifier>;

class CrudIdentifier {
  public:
    enum class Type { PRIMARY_KEY, NAME };

    STRINGABLE_ENUM(
        InputFormat,
        NONE,
        ID,
        NAME,
        ID_PARENT_ID,
        ID_PARENT_NAME,
        PARENT_NAME,
        PARENT_ID,
        NAME_PARENT_NAME,
        NAME_PARENT_ID)

    struct FormatSpec {
        std::string m_primary_key_name{"id"};
        std::string m_name_key_name{"name"};
        std::string m_parent_id_key_name{"parent_id"};
        std::string m_parent_name_key_name{"parent_name"};
        InputFormat m_input_format;
    };

    CrudIdentifier() = default;

    CrudIdentifier(const Map& buffer, const FormatSpec& format);

    CrudIdentifier(const std::string& buffer, const FormatSpec& format);

    CrudIdentifier(const std::string& buffer, Type type = Type::PRIMARY_KEY);

    CrudIdentifier(
        const std::string& name,
        const std::string& parent_key,
        Type type = Type::PRIMARY_KEY);

    const std::string& get_primary_key() const;

    const std::string& get_name() const;

    const std::string& get_parent_name() const;

    const std::string& get_parent_primary_key() const;

    bool has_value() const;

    bool has_primary_key() const;

    static InputFormat input_format_from_string(const std::string& format);

    bool has_name() const;

    bool has_parent_primary_key() const;

    bool has_parent_name() const;

    void set_primary_key(const std::string& key);

    void set_name(const std::string& name) { m_name = name; }

    void set_parent_name(const std::string& name) { m_parent_name = name; }

    void set_parent_primary_key(const std::string& id)
    {
        m_parent_primary_key = id;
    }

    void from_buffer(const std::string& line, InputFormat format);

    void write(Dictionary& dict, const FormatSpec& format) const;

  private:
    std::string m_primary_key;
    std::string m_name;
    std::string m_parent_primary_key;
    std::string m_parent_name;
};

}  // namespace hestia