#pragma once

#include "CrudIdentifierCollection.h"
#include "CrudRequest.h"
#include "CrudResponse.h"
#include "JsonDocument.h"
#include "ModelSerializer.h"

#include "ErrorUtils.h"

#include <stdexcept>

namespace hestia {
class CrudSerializer {
  public:
    struct FormatSpec {
        CrudAttributes::FormatSpec m_attr_format;
        CrudIdentifierCollection::FormatSpec m_id_format;
    };

    CrudSerializer(ModelSerializer::Ptr model_serializer);

    using Ptr = std::unique_ptr<CrudSerializer>;
    static Ptr create(ModelSerializer::Ptr model_serializer);

    std::string get_type() const;

    std::string get_runtime_type() const;

    bool type_has_owner() const;

    void load_template();

    ModelSerializer* get_model_serializer() const;

    Model::Ptr create_template() const;

    Model::Ptr create_template(const CrudRequest& req, std::size_t index) const;

    Model* get_template() const;

    void to_json(
        const CrudRequest& req,
        std::string& buffer,
        const Index& index = {}) const;

    void to_json(const Model& model, std::string& buffer) const;

    void append_to_dict(const Model& model, Dictionary& dict) const;

    void append_dict(
        Dictionary::Ptr dict,
        CrudResponse& response,
        bool record_modified = false) const;

    void append_json(const std::string& json_str, CrudResponse& response) const;

    bool matches_query(const Model& item, const Map& query) const;

  private:
    FormatSpec m_format;
    ModelSerializer::Ptr m_model_serializer;
};
}  // namespace hestia