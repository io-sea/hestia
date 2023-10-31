#pragma once

#include "CrudIdentifier.h"

namespace hestia {
class CrudIdentifierCollection {
  public:
    struct FormatSpec {
        CrudIdentifier::FormatSpec m_id_spec;
        char m_id_delimiter{'\n'};
        std::string m_end_delimiter{"\n\n"};
    };

    CrudIdentifierCollection() = default;

    CrudIdentifierCollection(const std::vector<std::string>& primary_keys);

    CrudIdentifierCollection(
        const std::vector<std::string>& keys,
        const CrudIdentifier::InputFormat& format);

    CrudIdentifierCollection(
        const std::string& buffer, const FormatSpec& format);

    const CrudIdentifier& first() const;

    const CrudIdentifier& id(std::size_t index) const;

    bool empty() const;

    const VecCrudIdentifier& data() const;

    std::size_t size() const;

    void add(const CrudIdentifier& id);

    void add_primary_key(const std::string& key);

    void add_primary_keys(const std::vector<std::string>& keys);

    void get_primary_keys(std::vector<std::string>& keys) const;

    void load(const Dictionary& dict, const CrudIdentifier::FormatSpec& format);

    void load(
        const std::vector<std::string>& buffer,
        const CrudIdentifier::FormatSpec& format);

    void load_primary_keys(const std::vector<std::string>& buffer);

    std::size_t load(const std::string& buffer, const FormatSpec& format);

    void write(
        Dictionary& dict, const CrudIdentifier::FormatSpec& format) const;

    void write(
        std::string& buffer,
        const CrudIdentifierCollection::FormatSpec& format) const;

  private:
    FormatSpec m_format;
    std::vector<CrudIdentifier> m_ids;
};
}  // namespace hestia