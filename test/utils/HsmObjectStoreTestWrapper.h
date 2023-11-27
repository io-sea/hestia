#pragma once

#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientPlugin.h"

class HsmObjectStoreTestWrapper {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreTestWrapper>;
    HsmObjectStoreTestWrapper(hestia::HsmObjectStoreClient::Ptr client);
    HsmObjectStoreTestWrapper(const std::string& plugin_name);

    static Ptr create(hestia::HsmObjectStoreClient::Ptr client);
    static Ptr create(const std::string& plugin_name);

    void put(
        const hestia::StorageObject& obj,
        const std::string& content,
        const std::string& tier);

    void get(hestia::StorageObject& obj);

    void get(
        hestia::StorageObject& obj,
        std::string& content,
        std::size_t content_length,
        const std::string& tier);

    void exists(const hestia::StorageObject& obj, bool should_exist);

    void list(
        const hestia::KeyValuePair& query,
        std::vector<hestia::StorageObject>& result);

    void copy(
        const hestia::StorageObject& obj,
        const std::string& source_tier,
        const std::string& target_tier);

    void move(
        const hestia::StorageObject& obj,
        const std::string& source_tier,
        const std::string& target_tier);

    void remove(const hestia::StorageObject& obj);

    hestia::HsmObjectStoreClient* m_client{nullptr};
    hestia::HsmObjectStoreClient::Ptr m_client_owner;
    hestia::HsmObjectStoreClientPlugin::Ptr m_client_plugin_owner;
};