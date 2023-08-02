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
    BaseObjectStoreResponse(
        const BaseObjectStoreRequest& request, const std::string& store_id) :
        Response<ErrorCode>(request),
        m_object(request.object()),
        m_store_id(store_id)
    {
    }

    virtual ~BaseObjectStoreResponse() = default;

    const std::string& get_store_id() const { return m_store_id; }

    StorageObject& object() { return m_object; }

    std::vector<StorageObject>& objects() { return m_objects; }

    bool object_found() const { return m_object_found; }

    void set_object_found(bool is_found) { m_object_found = is_found; }

    bool object_is_remote() const { return !m_object.get_location().empty(); }

  protected:
    StorageObject m_object;
    std::string m_store_id;

  private:
    std::vector<StorageObject> m_objects;
    bool m_object_found{false};
};

class ObjectStoreResponse :
    public BaseObjectStoreResponse<ObjectStoreErrorCode> {
  public:
    using Ptr = std::unique_ptr<ObjectStoreResponse>;

    ObjectStoreResponse(
        const BaseObjectStoreRequest& request, const std::string& store_id);

    static Ptr create(
        const BaseObjectStoreRequest& request, const std::string& store_id);

    virtual ~ObjectStoreResponse() = default;
};
}  // namespace hestia