#pragma once

#include "CrudQuery.h"
#include "CrudRequest.h"
#include "Model.h"
#include "Response.h"

#include <cassert>

namespace hestia {

class CrudResponse : public Response<CrudErrorCode> {
  public:
    using Ptr = std::unique_ptr<CrudResponse>;

    CrudResponse(
        const BaseRequest& request,
        const std::string& type,
        const CrudQuery::OutputFormat& output_format);

    static Ptr create(
        const BaseRequest& request,
        const std::string& type,
        const CrudQuery::OutputFormat& output_format);

    virtual ~CrudResponse();

    void add_attributes(
        const std::string& buffer, const CrudAttributes::FormatSpec& format);

    void add_attributes(
        std::unique_ptr<Dictionary> dict,
        const CrudIdentifier::FormatSpec& id_format);

    void add_ids(
        const std::string& json, const CrudIdentifier::FormatSpec& format);

    void add_item(std::unique_ptr<Model> item);

    const CrudAttributes& get_attributes() const;

    Model* get_item() const;

    template<typename T>
    const T* get_item_as() const
    {
        auto typed_model = dynamic_cast<const T*>(get_item());
        assert(typed_model != nullptr);
        return typed_model;
    }

    bool expects_modified_attributes() const;

    bool expects_attributes() const;

    bool expects_items() const;

    bool expects_ids() const;

    bool found() const;

    bool has_attributes() const;

    const VecModelPtr& items() const;

    VecModelPtr& items();

    CrudIdentifierCollection& ids();

    const CrudIdentifierCollection& get_ids() const;

    Dictionary& modified_attrs();

    const Dictionary& modified_attrs() const;

    void set_attributes(JsonDocument::Ptr json);

    void set_locked(bool is_locked);

    bool locked() const;

    void write(std::string& buffer, const CrudQuery::FormatSpec& format);

  private:
    VecModelPtr m_items;
    CrudAttributes m_attributes;
    CrudQuery::OutputFormat m_output_format;

    CrudIdentifierCollection m_ids;
    Dictionary m_modified_attrs;

    bool m_locked{false};
    std::string m_type;

  protected:
    RequestError<CrudErrorCode> m_error;
};

}  // namespace hestia