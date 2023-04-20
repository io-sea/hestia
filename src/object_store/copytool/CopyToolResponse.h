#pragma once

#include "CopyToolError.h"

#include "HsmMiddlewareResponse.h"
#include "HsmObjectStoreResponse.h"

class CopyToolResponse : public HsmMiddlewareResponse {
  public:
    using Ptr = std::unique_ptr<CopyToolResponse>;

    enum class Status { OK, ERROR };

    CopyToolResponse();
    virtual ~CopyToolResponse() = default;

    static Ptr Create();

    bool ok() const;

    void setError(const CopyToolError& error);

    void setObjectStoreRespose(
        HsmObjectStoreResponse::Ptr objectStoreResponse) override;

  private:
    Status mStatus{Status::OK};
    CopyToolError mError;
};