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

    BaseObjectStoreRequest(const KeyValuePair& query) : m_query(query) {}

    virtual ~BaseObjectStoreRequest() = default;

    const KeyValuePair& query() const { return m_query; }

    const Extent& extent() const { return m_extent; }

    void set_extent(const Extent& extent) { m_extent = extent; }

    StorageObject& object() { return m_object; }

    const StorageObject& object() const { return m_object; }

    std::size_t get_progress_interval() const { return m_progress_interval; }

    void set_progress_interval(std::size_t interval)
    {
        m_progress_interval = interval;
    }

  protected:
    StorageObject m_object;
    std::size_t m_progress_interval{0};

  private:
    Extent m_extent;
    KeyValuePair m_query;
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

    MethodObjectStoreRequest(const KeyValuePair& query, METHOD method) :
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
    ObjectStoreRequest(const KeyValuePair& query);

    virtual ~ObjectStoreRequest() = default;

    std::string method_as_string() const override;

    static std::string to_string(ObjectStoreRequestMethod method);
};
}  // namespace hestia