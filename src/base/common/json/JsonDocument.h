#pragma once

#include "Dictionary.h"
#include "Index.h"

namespace hestia {
class JsonDocumentImpl;

class JsonDocument {
  public:
    struct FormatSpec {
        FormatSpec() : m_indent(false) {}

        void set_indent(std::size_t indent)
        {
            m_indent_size = indent;
            m_indent      = true;
        }

        bool m_indent{false};
        std::size_t m_indent_size{4};
        bool m_collapse_single{false};
        std::vector<std::string> m_exclude_keys;
    };

    JsonDocument(const std::string& raw = {});
    using Ptr = std::unique_ptr<JsonDocument>;

    JsonDocument(const JsonDocument& other);

    JsonDocument(const Dictionary& dict);

    JsonDocument(const std::vector<std::string>& raw);

    static Ptr create(const std::string& raw = {});

    static Ptr create(const Dictionary& dict);

    ~JsonDocument();

    void append(const std::string& body);

    void append(
        const Dictionary& dict,
        const std::vector<std::string>& exclude_keys = {});

    void get_values(
        const SearchExpression& search, std::vector<std::string>& values) const;

    std::size_t get_size() const;

    bool is_empty() const;

    const JsonDocument& load(const std::string& body);

    const JsonDocument& load(
        const Dictionary& dict,
        const std::vector<std::string>& exclude_keys = {});

    std::string to_string(
        const Index& index = {}, const FormatSpec& format = {}) const;

    void write(
        Dictionary& dict,
        const Index& index       = {},
        const FormatSpec& format = {});

    void write(
        std::string& buffer,
        const Index& index       = {},
        const FormatSpec& format = {}) const;

  private:
    std::unique_ptr<JsonDocumentImpl> m_impl;
};
}  // namespace hestia