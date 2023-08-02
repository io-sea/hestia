#pragma once

#include "Map.h"
#include "ObjectStoreRequest.h"
#include "ObjectStoreResponse.h"
#include "Stream.h"

#include <vector>

namespace hestia {

/**
 * @brief Object Store Client abstraction
 *
 * This is an abstraction of an Object Store client - i.e. something that talks
 * to an Object Store backend. Derived types can be implemented to target
 * specific backends.
 *
 * Consumers work with the 'request-reponse' API by calling 'make_request' with
 * an ObjectStoreRequest and handling the resulting ObjectStoreResponse. This
 * does not throw, errors are included in the response.
 *
 * Implementers will override type 'put', 'get', 'remove' etc interfaces for a
 * specific backend. They are unlikely to need to override  'make_request'.
 * These methods are free to throw, preferably an
 * 'RequestException<ObjectStoreError>' type.
 */

class ObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<ObjectStoreClient>;

    ObjectStoreClient() = default;

    virtual ~ObjectStoreClient() = default;

    virtual void initialize(
        const std::string& id, const std::string&, const Dictionary&)
    {
        m_id = id;
    }

    /**
     * Make a request to the object store backend. Consumers should primarily
     * work with this method.
     *
     * @param request the request - which will have a Method type and payload
     * @param stream if object data is to be provided (e.g. PUT) or retrieved (e.g. GET) it will be via the Stream
     * @return a response - which can contain error info, a non-data payload and request metadata
     */
    [[nodiscard]] virtual ObjectStoreResponse::Ptr make_request(
        const ObjectStoreRequest& request,
        Stream* stream = nullptr) const noexcept;

  protected:
    virtual bool exists(const StorageObject& object) const = 0;

    virtual void list(
        const KeyValuePair& query,
        std::vector<StorageObject>& fetched) const = 0;

    virtual void get(
        StorageObject& object, const Extent& extent, Stream* stream) const = 0;

    virtual void put(
        const StorageObject& object,
        const Extent& extent,
        Stream* stream) const = 0;

    virtual void remove(const StorageObject& object) const = 0;

    std::string m_id;

  private:
    void on_exception(
        const ObjectStoreRequest& request,
        ObjectStoreResponse* response,
        const std::string& message = {}) const;

    void on_exception(
        const ObjectStoreRequest& request,
        ObjectStoreResponse* response,
        const ObjectStoreError& error) const;
};
}  // namespace hestia
