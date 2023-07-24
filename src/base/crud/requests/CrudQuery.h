#pragma once

#include "CrudAttributes.h"
#include "CrudIdentifier.h"

namespace hestia {

class CrudQuery {
  public:
    enum class OutputFormat { NONE, ATTRIBUTES, ID, ATTRIBUTES_AND_ID, ITEM };

    enum class InputFormat { Id, Filter, Index };

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
        const KeyValuePair& index,
        OutputFormat output_format = OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON);

    const Map& get_filter() const;

    const VecCrudIdentifier& ids() const;

    const CrudIdentifier& get_id() const;

    const KeyValuePair& get_index() const;

    OutputFormat get_output_format() const { return m_output_format; }

    bool has_single_id() const;

    bool is_filter() const;

    bool is_id() const;

    bool is_index() const;

    bool is_id_output_format() const
    {
        return m_output_format == OutputFormat::ID;
    }

    bool is_item_output_format() const
    {
        return m_output_format == OutputFormat::ITEM;
    }

    bool is_attribute_output_format() const
    {
        return m_output_format == OutputFormat::ATTRIBUTES;
    }

    void set_output_format(OutputFormat output_format)
    {
        m_output_format = output_format;
    }

    void set_attributes_output_format(CrudAttributes::Format format)
    {
        m_attributes_output_format = format;
    }

    void set_ids(const VecCrudIdentifier& ids) { m_input_ids = ids; }

    void set_index(const KeyValuePair& index) { m_input_index = index; }

    void set_offset(std::size_t offset) { m_offset = offset; }

    void set_count(std::size_t count) { m_count = count; }

    const CrudAttributes& get_attributes() const { return m_attributes; }

    std::vector<std::string> m_list_result;
    std::size_t m_max_items{1000};
    std::size_t m_offset{0};
    std::size_t m_count{0};

    CrudAttributes m_attributes;
    CrudAttributes::Format m_attributes_output_format{
        CrudAttributes::Format::JSON};

  private:
    OutputFormat m_output_format{OutputFormat::ATTRIBUTES};
    VecCrudIdentifier m_input_ids;
    KeyValuePair m_input_index;
    Map m_input_filter;
    InputFormat m_input_format{InputFormat::Filter};
};
}  // namespace hestia