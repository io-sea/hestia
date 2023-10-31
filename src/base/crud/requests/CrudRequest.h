#pragma once

#include "BaseCrudRequest.h"
#include "Model.h"

namespace hestia {

class CrudRequest : public BaseCrudRequest, public MethodRequest<CrudMethod> {
  public:
    CrudRequest() = default;

    CrudRequest(CrudMethod method);

    explicit CrudRequest(
        CrudMethod method,
        VecModelPtr items,
        const CrudQuery& query,
        const CrudUserContext& user_context);

    explicit CrudRequest(
        CrudMethod method,
        const CrudQuery& query,
        const CrudUserContext& user_context,
        bool update_event_feed = true);

    explicit CrudRequest(
        CrudMethod method, const CrudUserContext& user_context);

    explicit CrudRequest(
        CrudMethod method,
        CrudLockType lock_type,
        const CrudUserContext& user_context);

    virtual ~CrudRequest();

    using Ptr = std::unique_ptr<CrudRequest>;

    std::string method_as_string() const override;

    Model* get_item() const;

    Dictionary::Ptr get_attribute_as_dict(std::size_t) const;

    bool has_items() const;

    bool has_ids() const;

    bool has_attributes() const;

    bool is_create_method() const;

    bool is_read_method() const;

    bool is_identify_method() const;

    bool is_update_method() const;

    bool is_remove_method() const;

    bool is_create_or_update_method() const;

    const VecModelPtr& items() const;

    std::size_t size() const;

    bool should_update_event_feed() const { return m_update_event_feed; }

    void write_body(
        const CrudAttributes::FormatSpec& format,
        std::string& buffer,
        const Index& index = {}) const;

  protected:
    bool m_update_event_feed{true};
    VecModelPtr m_items;
};
}  // namespace hestia