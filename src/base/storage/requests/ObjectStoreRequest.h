#pragma once

#include "Extent.h"
#include "Request.h"
#include "StorageObject.h"

namespace hestia {
class BaseObjectStoreRequest : public BaseRequest {
  public:
    BaseObjectStoreRequest(const StorageObject& object) : m_object(object) {}

    BaseObjectStoreRequest(const std::string& object_id) :
        m_object(StorageObject(object_id))
    {
    }

    BaseObjectStoreRequest(const Metadata::Query& query) : m_query(query) {}

    virtual ~BaseObjectStoreRequest() = default;

    const Metadata::Query& query() const { return m_query; }

    const Extent& extent() const { return m_extent; }

    void set_extent(const Extent& extent) { m_extent = extent; }

    StorageObject& object() { return m_object; }

    const StorageObject& object() const { return m_object; }

    bool should_overwrite() const { return m_overwrite; }

  protected:
    StorageObject m_object;

  private:
    Extent m_extent;
    Metadata::Query m_query;
    bool m_overwrite{false};
};

template<typename METHOD>
class MethodObjectStoreRequest :
    public MethodRequest<METHOD>,
    public BaseObjectStoreRequest {
  public:
    MethodObjectStoreRequest(const StorageObject& object, METHOD method) :
        MethodRequest<METHOD>(method), BaseObjectStoreRequest(object)
    {
    }

    MethodObjectStoreRequest(const std::string& object_id, METHOD method) :
        MethodRequest<METHOD>(method),
        BaseObjectStoreRequest(StorageObject(object_id))
    {
    }

    MethodObjectStoreRequest(const Metadata::Query& query, METHOD method) :
        MethodRequest<METHOD>(method), BaseObjectStoreRequest(query)
    {
    }

    MethodObjectStoreRequest(
        const BaseObjectStoreRequest& other, METHOD method) :
        MethodRequest<METHOD>(method), BaseObjectStoreRequest(other)
    {
    }

    virtual ~MethodObjectStoreRequest() = default;
};

enum class ObjectStoreRequestMethod { EXISTS, GET, LIST, PUT, REMOVE, CUSTOM };

class ObjectStoreRequest :
    public MethodObjectStoreRequest<ObjectStoreRequestMethod> {
  public:
    ObjectStoreRequest(
        const StorageObject& object, ObjectStoreRequestMethod method);
    ObjectStoreRequest(
        const std::string& object_id, ObjectStoreRequestMethod method);
    ObjectStoreRequest(const Metadata::Query& query);

    virtual ~ObjectStoreRequest() = default;

    std::string method_as_string() const override;

    static std::string to_string(ObjectStoreRequestMethod method);
};
}  // namespace hestia