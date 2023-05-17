#pragma once

#include "KeyValueStoreError.h"
#include "KeyValueStoreRequest.h"
#include "Response.h"

#include <memory>
#include <string>
#include <vector>

namespace hestia {
class KeyValueStoreResponse : public Response<KeyValueStoreErrorCode> {
  public:
    using Ptr = std::unique_ptr<KeyValueStoreResponse>;
    KeyValueStoreResponse(const KeyValueStoreRequest& request);

    virtual ~KeyValueStoreResponse() = default;

    static Ptr create(const KeyValueStoreRequest& request);

    bool has_key() const;

    const std::string& get_value() const;

    std::string& get_value();

    const std::vector<std::string>& get_set_items() const;

    std::vector<std::string>& get_set_items();

    void set_value(const std::string& value);

    void set_key_found(bool found);

  private:
    bool m_key_found{false};
    std::string m_value;
    std::vector<std::string> m_set_items;
};
}  // namespace hestia