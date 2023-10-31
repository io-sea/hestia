#pragma once

#include "Dictionary.h"
#include "EnumUtils.h"
#include "JsonDocument.h"
#include "SearchExpression.h"

namespace hestia {

class CrudAttributes {
  public:
    STRINGABLE_ENUM(Format, NONE, JSON, DICT, UNSET)

    struct FormatSpec {
        FormatSpec() : m_type(Format::UNSET) {}

        static Format from_string(const std::string& format);

        static std::string to_string(Format format);

        bool is_none_type() const;

        bool is_unset() const;

        bool is_json() const;

        void set_is_dict();

        void set_is_json();

        Format m_type{Format::UNSET};
        JsonDocument::FormatSpec m_json_format;
        Dictionary::FormatSpec m_dict_format;
    };

    CrudAttributes();

    CrudAttributes(const std::string& body, const FormatSpec& format);

    CrudAttributes(const Map& map, const FormatSpec& format);

    CrudAttributes(JsonDocument::Ptr json);

    CrudAttributes(Dictionary::Ptr dict);

    CrudAttributes(const CrudAttributes& other);

    void append(const std::string& buffer, const FormatSpec& format);

    JsonDocument::Ptr get_copy_as_json() const;

    Dictionary::Ptr get_copy_as_dict() const;

    Dictionary::Ptr get_copy_as_dict(std::size_t index) const;

    std::vector<std::string> get_ids() const;

    JsonDocument* get_json() const;

    Dictionary* get_dict() const;

    void get_values(
        const SearchExpression& search, std::vector<std::string>& values) const;

    Format get_format() const;

    std::string get_format_as_string() const;

    const std::string& get_key_prefix() const;

    std::size_t get_size() const;

    bool has_content() const;

    bool is_json() const;

    void set_body(Dictionary::Ptr dict);

    void set_body(JsonDocument::Ptr json);

    void set_format(const FormatSpec& format);

    void set_key_prefix(const std::string& prefix);

    std::string to_string(
        const FormatSpec& format = {}, const Index& index = {}) const;

    void write(
        std::string& output,
        const FormatSpec& format = {},
        const Index& index       = {}) const;

  private:
    Dictionary::Ptr m_dict;
    JsonDocument::Ptr m_json;

    std::string m_key_prefix;
    FormatSpec m_format;
};
}  // namespace hestia