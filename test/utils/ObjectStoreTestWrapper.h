#pragma once

#include "ObjectStoreClient.h"
#include "ObjectStoreClientPlugin.h"

class ObjectStoreTestWrapper {
  public:
    using Ptr = std::unique_ptr<ObjectStoreTestWrapper>;
    ObjectStoreTestWrapper(hestia::ObjectStoreClient::Ptr client);
    ObjectStoreTestWrapper(const std::string& plugin_name);

    static Ptr create(hestia::ObjectStoreClient::Ptr client);
    static Ptr create(const std::string& plugin_name);

    void put(
        const hestia::StorageObject& obj, hestia::Stream* stream = nullptr);

    void get(hestia::StorageObject& obj, hestia::Stream* stream = nullptr);

    void exists(const hestia::StorageObject& obj, bool should_exist);

    void list(
        const hestia::KeyValuePair& query,
        std::vector<hestia::StorageObject>& result);

    void remove(const hestia::StorageObject& obj);

    hestia::ObjectStoreClient* m_client{nullptr};
    hestia::ObjectStoreClient::Ptr m_client_owner;
    hestia::ObjectStoreClientPlugin::Ptr m_client_plugin_owner;
};