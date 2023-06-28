#pragma once

#include "Request.h"
#include "RequestError.h"
#include "RequestException.h"
#include "Response.h"

#include "HsmNode.h"

namespace hestia {
enum class DistributedHsmServiceRequestMethod { PUT, GET, LIST };

class DistributedHsmServiceRequest :
    public BaseRequest,
    public MethodRequest<DistributedHsmServiceRequestMethod> {
  public:
    using Ptr = std::unique_ptr<DistributedHsmServiceRequest>;

    DistributedHsmServiceRequest(
        const HsmNode& item, DistributedHsmServiceRequestMethod method);

    DistributedHsmServiceRequest(DistributedHsmServiceRequestMethod method);

    DistributedHsmServiceRequest(const Metadata& query);

    std::string method_as_string() const override;

    const HsmNode& item() const;

    const Metadata& query() const;

  private:
    Metadata m_query;
    HsmNode m_item;
};

enum class DistributedHsmServiceErrorCode {
    NO_ERROR,
    ERROR,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    OBJECT_NOT_FOUND,
    BAD_PUT_OVERWRITE_COMBINATION,
    MAX_ERROR
};

class DistributedHsmServiceError :
    public RequestError<DistributedHsmServiceErrorCode> {
  public:
    DistributedHsmServiceError();
    DistributedHsmServiceError(
        DistributedHsmServiceErrorCode code, const std::string& message);
    virtual ~DistributedHsmServiceError() = default;

  private:
    std::string code_as_string() const override;
    static std::string code_to_string(DistributedHsmServiceErrorCode code);
};

class DistributedHsmServiceResponse :
    public Response<DistributedHsmServiceErrorCode> {
  public:
    using Ptr = std::unique_ptr<DistributedHsmServiceResponse>;

    DistributedHsmServiceResponse(const DistributedHsmServiceRequest& request);

    DistributedHsmServiceResponse(
        const DistributedHsmServiceRequest& request,
        std::unique_ptr<CrudResponse<HsmNode, CrudErrorCode>> crud_response);

    static Ptr create(const DistributedHsmServiceRequest& request);

    static Ptr create(
        const DistributedHsmServiceRequest& request,
        std::unique_ptr<CrudResponse<HsmNode, CrudErrorCode>> crud_response);

    const HsmNode& item() const;

    const std::vector<HsmNode>& items() const;

    std::vector<HsmNode>& items();

  private:
    std::unique_ptr<CrudResponse<HsmNode, CrudErrorCode>> m_crud_response;

    HsmNode m_item;
    std::vector<HsmNode> m_items;
};
}  // namespace hestia