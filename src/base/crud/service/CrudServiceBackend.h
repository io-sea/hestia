#pragma once

#include "HttpClient.h"
#include "KeyValueStoreClient.h"

namespace hestia {

class CrudServiceBackend {
  public:
    enum class Type { HTTP_REST, KEY_VALUE_STORE, RELATIONAL_DATABASE };

    virtual ~CrudServiceBackend() = default;

    Type get_type() const { return m_type; }
    Type m_type{Type::KEY_VALUE_STORE};
};

class KeyValueStoreCrudServiceBackend : public CrudServiceBackend {
  public:
    KeyValueStoreCrudServiceBackend(KeyValueStoreClient* client) :
        m_client(client)
    {
        m_type = Type::KEY_VALUE_STORE;
    }
    KeyValueStoreClient* m_client{nullptr};
};

class HttpRestCrudServiceBackend : public CrudServiceBackend {
  public:
    HttpRestCrudServiceBackend(HttpClient* client) : m_client(client)
    {
        m_type = Type::HTTP_REST;
    }
    HttpClient* m_client{nullptr};
};
}  // namespace hestia