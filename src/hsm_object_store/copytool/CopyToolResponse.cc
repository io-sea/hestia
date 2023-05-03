#include "CopyToolResponse.h"

namespace hestia {
CopyToolResponse::CopyToolResponse() : HsmMiddlewareResponse() {}

CopyToolResponse::Ptr CopyToolResponse::create()
{
    return std::make_unique<CopyToolResponse>();
}

bool CopyToolResponse::ok() const
{
    return m_status != Status::ERROR;
};

void CopyToolResponse::set_error(const CopyToolError& error)
{
    m_error = error;
}

void CopyToolResponse::set_object_store_respose(
    HsmObjectStoreResponse::Ptr object_store_response)
{
    m_object_store_response = std::move(object_store_response);

    if (!m_object_store_response->ok()) {
        const std::string msg =
            "Object store failed with error: "
            + m_object_store_response->get_error().to_string();
        set_error({CopyToolError::Code::ERROR_OBJECT_STORE, msg});
    }
}
}  // namespace hestia