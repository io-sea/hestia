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

    CrudResponse(const BaseRequest& request, const std::string& type);

    static Ptr create(const BaseRequest& request, const std::string& type);

    virtual ~CrudResponse();

    const CrudAttributes& attributes() const;

    CrudAttributes& attributes();

    Model* get_item() const;

    template<typename T>
    const T* get_item_as() const
    {
        auto typed_model = dynamic_cast<const T*>(get_item());
        assert(typed_model != nullptr);
        return typed_model;
    }

    const VecModelPtr& items() const;

    VecModelPtr& items();

    const Dictionary* dict() const;

    std::vector<std::string>& ids() { return m_ids; }

    const std::vector<std::string>& ids() const { return m_ids; }

    std::vector<std::string>& parent_ids() { return m_parent_ids; }

    std::vector<Map>& modified_attrs() { return m_modified_attrs; };

    const std::vector<Map>& modified_attrs() const { return m_modified_attrs; };

    bool found() const;

    void set_item(std::unique_ptr<Model> item);

    void set_dict(std::unique_ptr<Dictionary> dict);

    void set_locked(bool is_locked);

    bool locked() const;

  private:
    VecModelPtr m_items;
    std::unique_ptr<Dictionary> m_items_dict;
    CrudAttributes m_attributes;

    std::vector<std::string> m_ids;
    std::vector<std::string> m_parent_ids;

    std::vector<Map> m_modified_attrs;

    bool m_locked{false};
    std::string m_type;

  protected:
    RequestError<CrudErrorCode> m_error;
};

}  // namespace hestia