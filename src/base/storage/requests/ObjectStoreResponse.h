#pragma once

#include "ObjectStoreError.h"
#include "ObjectStoreRequest.h"
#include "Response.h"

#include <memory>
#include <vector>

namespace hestia {

template<typename ErrorCode>
class BaseObjectStoreResponse : public Response<ErrorCode> {
  public:
    BaseObjectStoreResponse(const BaseObjectStoreRequest& request) :
        Response<ErrorCode>(request), m_object(request.object())
    {
    }

    virtual ~BaseObjectStoreResponse() = default;

    StorageObject& object() { return m_object; }

    std::vector<StorageObject>& objects() { return m_objects; }

    bool object_found() const { return m_object_found; }

    void set_object_found(bool is_found) { m_object_found = is_found; }

  protected:
    StorageObject m_object;

  private:
    std::vector<StorageObject> m_objects;
    bool m_object_found{false};
};

class ObjectStoreResponse :
    public BaseObjectStoreResponse<ObjectStoreErrorCode> {
  public:
    using Ptr = std::unique_ptr<ObjectStoreResponse>;

    ObjectStoreResponse(const BaseObjectStoreRequest& request);

    static Ptr create(const BaseObjectStoreRequest& request);

    virtual ~ObjectStoreResponse() = default;
};
}  // namespace hestia