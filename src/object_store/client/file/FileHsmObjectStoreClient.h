#pragma once

#include "HsmObjectStoreClient.h"

#include <filesystem>

struct FileHsmObjectStoreClientConfig
{
    std::filesystem::directory_entry mStoreLocation{"object_store"};
};

class FileHsmObjectStoreClient : public HsmObjectStoreClient
{
public:
    using Ptr = std::unique_ptr<FileHsmObjectStoreClient>;

    FileHsmObjectStoreClient();

    ~FileHsmObjectStoreClient();

    static Ptr Create();

    static std::string getRegistryIdentifier();

    void initialize(const FileHsmObjectStoreClientConfig& config);

private:
    void put(const HsmObjectStoreRequest& request, ostk::Stream* stream) const override;

    void get(const HsmObjectStoreRequest& request, ostk::StorageObject& object, ostk::Stream* stream) const override;

    void remove(const HsmObjectStoreRequest& request) const override;

    void copy(const HsmObjectStoreRequest& request) const override;

    void move(const HsmObjectStoreRequest& request) const override;

    std::filesystem::path getTierPath(uint8_t tier) const;
    std::filesystem::directory_entry mStore;
};