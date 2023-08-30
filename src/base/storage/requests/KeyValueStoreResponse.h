#pragma once

#include "RequestError.h"
#include "Response.h"

#include <memory>

namespace hestia {

class KeyValueStoreResponse : public Response<CrudErrorCode> {
  public:
    KeyValueStoreResponse(const BaseRequest& request);

    virtual ~KeyValueStoreResponse();

    using Ptr = std::unique_ptr<KeyValueStoreResponse>;

    const std::vector<std::vector<std::string>>& ids() const;

    std::vector<std::vector<std::string>>& ids();

    const std::vector<std::string>& get_items() const;

    std::vector<std::string>& items();

    const std::vector<bool>& found() const;

    std::vector<bool>& found();

    void set_locked(bool is_locked);

    bool locked() const;

  private:
    std::vector<std::vector<std::string>> m_ids;
    std::vector<std::string> m_items;
    std::vector<bool> m_found;

    bool m_locked{false};

  protected:
    RequestError<CrudErrorCode> m_error;
};

}  // namespace hestia