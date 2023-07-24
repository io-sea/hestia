#pragma once

#include "CrudQuery.h"
#include "CrudRequest.h"
#include "Model.h"
#include "Response.h"

#include <cassert>

namespace hestia {

class CrudResponse : public Response<CrudErrorCode> {
  public:
    CrudResponse(const BaseRequest& request);

    virtual ~CrudResponse();

    using Ptr = std::unique_ptr<CrudResponse>;

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

    std::vector<std::string>& ids() { return m_ids; }

    bool found() const;

    void set_item(std::unique_ptr<Model>& item);

    void set_locked(bool is_locked);

    bool locked() const;

  private:
    VecModelPtr m_items;
    CrudAttributes m_attributes;
    std::vector<std::string> m_ids;

    bool m_locked{false};

  protected:
    RequestError<CrudErrorCode> m_error;
};

}  // namespace hestia