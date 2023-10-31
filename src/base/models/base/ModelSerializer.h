#pragma once

#include "Dictionary.h"
#include "Model.h"

namespace hestia {

class ModelSerializer {
  public:
    ModelSerializer() = default;

    ModelSerializer(
        ModelFactory::Ptr model_factory,
        Serializeable::Format format = Serializeable::Format::FULL);

    using Ptr = std::unique_ptr<ModelSerializer>;

    virtual ~ModelSerializer();

    Model::Ptr create_template() const { return m_model_factory->create(); }

    std::string get_type() const { return m_model_factory->get_type(); }

    std::string get_runtime_type() const
    {
        return m_template->get_runtime_type();
    }

    bool type_has_owner() const { return m_template->has_owner(); }

    Model* get_template() const;

    void load_template();

    virtual void to_dict(
        const VecModelPtr& items,
        Dictionary& output,
        const std::vector<Dictionary>& overrides = {},
        const Index& index                       = {},
        Serializeable::Format format_in = Serializeable::Format::UNSET) const;

    virtual void to_dict(
        const Model& item,
        Dictionary& output,
        const Dictionary& override      = {},
        Serializeable::Format format_in = Serializeable::Format::UNSET) const;


    virtual void from_dict(const Dictionary& input, VecModelPtr& items) const;

    virtual bool matches_query(const Model& item, const Map& query) const;

  protected:
    Model::Ptr m_template;
    ModelFactory::Ptr m_model_factory{nullptr};
    Serializeable::Format m_format{Serializeable::Format::FULL};

    std::vector<std::string> m_serializer_exclude_keys;
    std::vector<std::string> m_deserializer_exclude_keys;
};

}  // namespace hestia