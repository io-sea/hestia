#pragma once

#include <string>
#include <vector>

namespace hestia {

class CrudIdentifier {
  public:
    enum class Type { PRIMARY_KEY, NAME };

    enum class InputFormat {
        NONE,
        ID,
        NAME,
        ID_PARENT_ID,
        ID_PARENT_NAME,
        NAME_PARENT_NAME,
        NAME_PARENT_ID
    };

    CrudIdentifier() = default;

    CrudIdentifier(const std::string& key, Type type = Type::PRIMARY_KEY);

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

    bool has_name() const;

    bool has_parent_primary_key() const;

    bool has_parent_name() const;

    static std::size_t parse(
        const std::string& buffer,
        InputFormat format,
        std::vector<CrudIdentifier>& ids);

    void set_primary_key(const std::string& key);

    void set_name(const std::string& name) { m_name = name; }

    void set_parent_name(const std::string& name) { m_parent_name = name; }

    void set_parent_primary_key(const std::string& id)
    {
        m_parent_primary_key = id;
    }

  private:
    void from_buffer(const std::string& line, InputFormat format);

    std::string m_primary_key;
    std::string m_name;
    std::string m_parent_primary_key;
    std::string m_parent_name;
};

using VecCrudIdentifier = std::vector<CrudIdentifier>;
}  // namespace hestia