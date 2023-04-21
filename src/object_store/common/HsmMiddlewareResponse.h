#pragma once

#include <memory>

class HsmObjectStoreResponse;

class HsmMiddlewareResponse {
  public:
    using Ptr = std::unique_ptr<HsmMiddlewareResponse>;
    virtual ~HsmMiddlewareResponse();

    HsmObjectStoreResponse* get_object_store_response() const;

    virtual void set_object_store_respose(
        std::unique_ptr<HsmObjectStoreResponse> object_store_response) = 0;

  protected:
    std::unique_ptr<HsmObjectStoreResponse> m_object_store_response;
};