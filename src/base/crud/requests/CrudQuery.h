#pragma once

#include "CrudAttributes.h"
#include "CrudIdentifierCollection.h"

#include "EnumUtils.h"

namespace hestia {

class CrudQuery {
  public:
    enum class BodyFormat { NONE, JSON, DICT, ID, ITEM };
    enum class ChildFormat { NONE, ID, FULL };
    STRINGABLE_ENUM(Format, ID, GET, LIST)

    struct OutputFormat {
        OutputFormat() :
            m_body_format(BodyFormat::JSON), m_child_format(ChildFormat::FULL)
        {
        }

        OutputFormat(
            BodyFormat body_format,
            ChildFormat child_format = ChildFormat::FULL) :
            m_body_format(body_format), m_child_format(child_format)
        {
        }
        BodyFormat m_body_format   = BodyFormat::JSON;
        ChildFormat m_child_format = ChildFormat::FULL;
    };

    struct FormatSpec {
        CrudIdentifierCollection::FormatSpec m_id_format;
        CrudAttributes::FormatSpec m_attrs_format;
    };

    CrudQuery(const OutputFormat& output_format = {});

    CrudQuery(
        const CrudIdentifier& identifier,
        const OutputFormat& output_format = {});

    CrudQuery(
        const CrudIdentifierCollection& identifiers,
        const OutputFormat& output_format = {});

    CrudQuery(
        const Map& filter,
        const OutputFormat& output_format,
        const Format format = Format::LIST);

    CrudQuery(const CrudAttributes& attrs, const OutputFormat& output_format);

    CrudQuery(
        const std::string& attrs_body,
        const CrudAttributes::FormatSpec& attrs_format,
        const OutputFormat& output_format);

    CrudQuery(
        const CrudIdentifier& identifier,
        const CrudAttributes& attrs,
        const OutputFormat& output_format);

    CrudIdentifierCollection& ids();

    CrudAttributes& attributes();

    void append(
        const std::string& body, const CrudAttributes::FormatSpec& format);

    bool expects_single_item() const;

    static Format format_from_string(const std::string& input);

    const CrudAttributes& get_attributes() const;

    const Map& get_filter() const;

    Format get_format() const;

    std::string get_format_as_string() const;

    const CrudIdentifierCollection& get_ids() const { return m_ids; }

    const OutputFormat& get_output_format() const;

    std::size_t get_max_items() const;

    std::size_t get_offset() const;

    bool is_filter() const;

    bool is_id() const;

    bool is_id_output_format() const;

    bool is_item_output_format() const;

    bool is_attribute_output_format() const;

    bool is_json_output_format() const;

    bool is_dict_output_format() const;

    void set_format(Format format) { m_format = format; }

    void set_output_format(const OutputFormat& output_format);

    void set_ids(const CrudIdentifierCollection& ids);

    void set_ids(
        const std::string& buffer,
        const CrudIdentifierCollection::FormatSpec& format);

    void set_filter(const Map& filter);

    void set_offset(std::size_t offset);

    void set_count(std::size_t count);

  private:
    CrudAttributes m_attributes;

    std::size_t m_max_items{1000};
    std::size_t m_offset{0};
    std::size_t m_count{0};

    OutputFormat m_output_format;
    Format m_format{Format::LIST};

    CrudIdentifierCollection m_ids;
    Map m_filter;
};
}  // namespace hestia