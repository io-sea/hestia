#pragma once

#include "CrudAttributes.h"
#include "CrudIdentifier.h"

#include "EnumUtils.h"

namespace hestia {

class CrudQuery {
  public:
    enum class OutputFormat { ATTRIBUTES, ID, ITEM, DICT };
    STRINGABLE_ENUM(Format, ID, GET, LIST)

    CrudQuery(
        OutputFormat output_format = OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON);

    CrudQuery(
        const CrudIdentifier& identifier,
        OutputFormat output_format = OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON);

    CrudQuery(
        const VecCrudIdentifier& identifiers,
        OutputFormat output_format = OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON);

    CrudQuery(
        const Map& filter,
        Format format              = Format::LIST,
        OutputFormat output_format = OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON);

    CrudQuery(
        const KeyValuePair& filter,
        Format format              = Format::LIST,
        OutputFormat output_format = OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON);

    const VecCrudIdentifier& ids() const;

    const CrudAttributes& get_attributes() const;

    CrudAttributes& attributes();

    bool expects_single_item() const;

    static Format format_from_string(const std::string& input);

    const Map& get_filter() const;

    Format get_format() const;

    std::string get_format_as_string() const;

    void set_format(Format format) { m_format = format; }

    const CrudIdentifier& get_id() const;

    const VecCrudIdentifier& get_ids() const { return m_ids; }

    OutputFormat get_output_format() const;

    std::size_t get_max_items() const;

    std::size_t get_offset() const;

    bool has_single_id() const;

    bool is_filter() const;

    bool is_id() const;

    bool is_id_output_format() const;

    bool is_item_output_format() const;

    bool is_attribute_output_format() const;

    bool is_dict_output_format() const;

    void set_output_format(OutputFormat output_format);

    void set_attributes_output_format(CrudAttributes::Format format);

    void set_ids(const VecCrudIdentifier& ids);

    void set_filter(const Map& filter);

    void set_offset(std::size_t offset);

    void set_count(std::size_t count);

  private:
    CrudAttributes m_attributes;

    std::size_t m_max_items{1000};
    std::size_t m_offset{0};
    std::size_t m_count{0};

    Format m_format{Format::LIST};
    CrudAttributes::Format m_attributes_output_format{
        CrudAttributes::Format::JSON};
    OutputFormat m_output_format{OutputFormat::ATTRIBUTES};

    VecCrudIdentifier m_ids;
    Map m_filter;
};
}  // namespace hestia