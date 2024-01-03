#pragma once

#include "Map.h"
#include "ObjectStoreContext.h"

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
        const std::string& id, const std::string&, const Dictionary&);

    /**
     * Make a request to the object store backend. Consumers should primarily
     * work with this method.
     */
    virtual void make_request(ObjectStoreContext& ctx) const noexcept;

  protected:
    virtual bool exists(const StorageObject& object) const = 0;

    virtual void list(
        const KeyValuePair& query,
        std::vector<StorageObject>& fetched) const = 0;

    virtual void get(ObjectStoreContext& ctx) const = 0;

    virtual void put(ObjectStoreContext& ctx) const = 0;

    virtual void remove(const StorageObject& object) const = 0;

    virtual void init_stream(ObjectStoreContext& ctx) const;

    virtual void init_stream(
        ObjectStoreContext& ctx, const StorageObject& object) const;

    virtual void add_stream_progress_func(ObjectStoreContext& ctx) const;

    void on_object_not_found(
        const std::string& source_loc, const std::string& object_id) const;

    void on_success(const ObjectStoreContext& ctx) const;

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
