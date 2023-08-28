#pragma once

#include "Dictionary.h"
#include "Model.h"

namespace hestia {

class StringAdapter {
  public:
    StringAdapter(
        ModelFactory* model_factory,
        Serializeable::Format format = Serializeable::Format::FULL);

    virtual ~StringAdapter();

    virtual void to_dict(
        const VecModelPtr& items,
        Dictionary& output,
        const std::vector<Dictionary>& overrides = {},
        int index                                = -1,
        Serializeable::Format format_in = Serializeable::Format::UNSET) const;

    virtual void to_dict(
        const Model& item,
        Dictionary& output,
        const Dictionary& override      = {},
        Serializeable::Format format_in = Serializeable::Format::UNSET) const;

    virtual void to_string(
        const VecModelPtr& items,
        std::string& output,
        const std::vector<Dictionary>& overrides = {},
        int index                                = -1,
        Serializeable::Format format_in = Serializeable::Format::UNSET) const;

    virtual void to_string(
        const Model& item,
        std::string& output,
        const Dictionary& override      = {},
        Serializeable::Format format_in = Serializeable::Format::UNSET) const;

    virtual void from_string(
        const std::vector<std::string>& input,
        VecModelPtr& items,
        bool as_list = false) const;

    virtual void from_string(
        const std::vector<std::string>& input,
        Dictionary& dict,
        bool as_list = false) const;

    virtual void from_dict(const Dictionary& input, VecModelPtr& items) const;

    virtual bool matches_query(const Model& item, const Map& query) const;

    virtual void dict_from_string(
        const std::string& input,
        Dictionary& dict,
        const std::string& key_prefix = {}) const = 0;

    virtual void dict_to_string(
        const Dictionary& dict, std::string& output) const = 0;

  protected:
    ModelFactory* m_model_factory{nullptr};
    Serializeable::Format m_format{Serializeable::Format::FULL};

    std::vector<std::string> m_serializer_exclude_keys;
    std::vector<std::string> m_deserializer_exclude_keys;
};

class JsonAdapter : public StringAdapter {
  public:
    JsonAdapter(
        ModelFactory* model_factory,
        Serializeable::Format format = Serializeable::Format::FULL);

    void dict_to_string(
        const Dictionary& dict, std::string& output) const override;

    void dict_from_string(
        const std::string& input,
        Dictionary& dict,
        const std::string& key_prefix = {}) const override;
};

class KeyValueAdapter : public StringAdapter {
  public:
    KeyValueAdapter(
        ModelFactory* model_factory,
        Serializeable::Format format = Serializeable::Format::FULL);

    void dict_to_string(
        const Dictionary& dict, std::string& output) const override;

    void dict_from_string(
        const std::string& input,
        Dictionary& dict,
        const std::string& key_prefix = {}) const override;
};

class AdapterCollection {
  public:
    using Ptr = std::unique_ptr<AdapterCollection>;

    AdapterCollection(std::unique_ptr<ModelFactory> model_factory);

    const ModelFactory* get_model_factory() const;

    void add_adapter(
        const std::string& label, std::unique_ptr<StringAdapter> adapter);

    std::string get_type() const;

    StringAdapter* get_adapter(const std::string& label) const;

    StringAdapter* get_default_adapter() const;

  private:
    std::string m_default_adapter;
    std::unique_ptr<ModelFactory> m_model_factory;
    std::unordered_map<std::string, std::unique_ptr<StringAdapter>> m_adapters;
};

}  // namespace hestia