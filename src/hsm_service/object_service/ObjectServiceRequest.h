#pragma once

#include "HsmObject.h"
#include "Request.h"
#include "Response.h"
#include "StorageObject.h"
#include "StorageTier.h"

namespace hestia {
enum class ObjectServiceRequestMethod { PUT, GET, EXISTS, REMOVE, LIST };

class ObjectServiceRequest :
    public BaseRequest,
    public MethodRequest<ObjectServiceRequestMethod> {
  public:
    using Ptr = std::unique_ptr<ObjectServiceRequest>;

    ObjectServiceRequest(
        const HsmObject& object, ObjectServiceRequestMethod method);

    std::string method_as_string() const override;

    std::string to_string() const;

    const HsmObject& object() const { return m_object; }

  private:
    HsmObject m_object;
    std::string m_query;
};

enum class ObjectServiceErrorCode {
    NO_ERROR,
    ERROR,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    OBJECT_NOT_FOUND,
    MAX_ERROR
};

class ObjectServiceError : public RequestError<ObjectServiceErrorCode> {
  public:
    ObjectServiceError();
    ObjectServiceError(ObjectServiceErrorCode code, const std::string& message);
    virtual ~ObjectServiceError() = default;

  private:
    std::string code_as_string() const override;
    static std::string code_to_string(ObjectServiceErrorCode code);
};

class ObjectServiceResponse : public Response<ObjectServiceErrorCode> {
  public:
    using Ptr = std::unique_ptr<ObjectServiceResponse>;

    ObjectServiceResponse(const ObjectServiceRequest& request);

    static Ptr create(const ObjectServiceRequest& request);

    const std::string& query_result() const;

    const std::vector<HsmObject>& objects() const;

    std::vector<HsmObject>& objects() { return m_objects; }

    const HsmObject& object() const;

    HsmObject& object() { return m_object; }

    bool object_found() const { return m_object_found; }

    void set_object_found(bool found) { m_object_found = found; }

  private:
    std::string m_query_result;
    std::vector<HsmObject> m_objects;
    HsmObject m_object;
    bool m_object_found{false};
};
}  // namespace hestia
