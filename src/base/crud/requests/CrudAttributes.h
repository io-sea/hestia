#pragma once

#include "Map.h"

namespace hestia {
class CrudAttributes {

  public:
    enum class Format { NONE, JSON, KV_PAIR };

    CrudAttributes() = default;

    CrudAttributes(const std::string& buffer, Format format = Format::JSON);

    const std::string& get_buffer() const;

    std::string& buffer() { return m_buffer; }

    bool is_json() const;

    bool has_content() const;

    Format get_format() const { return m_format; }

    void set_buffer(const std::string& buffer, Format format = Format::JSON)
    {
        m_format = format;
        m_buffer = buffer;
    }

    static std::string to_string(Format format)
    {
        return format == Format::JSON ? "json" : "kv_pair";
    }

  private:
    std::string m_buffer;
    Format m_format{Format::JSON};
};
}  // namespace hestia