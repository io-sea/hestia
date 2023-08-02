#include "FileHsmObjectStoreClient.h"

#include "FileObjectStoreClient.h"
#include "FileUtils.h"
#include "RequestException.h"

#include "Logger.h"

namespace hestia {
FileHsmObjectStoreClient::FileHsmObjectStoreClient()
{
    LOG_INFO("Created");
}

FileHsmObjectStoreClient::~FileHsmObjectStoreClient()
{
    LOG_INFO("Destroyed");
}

std::string FileHsmObjectStoreClient::get_registry_identifier()
{
    return "hestia::FileHsmObjectStoreClient";
}

FileHsmObjectStoreClient::Ptr FileHsmObjectStoreClient::create()
{
    return std::make_unique<FileHsmObjectStoreClient>();
}

void FileHsmObjectStoreClient::initialize(
    const std::string& id,
    const std::string& cache_path,
    const Dictionary& config_data)
{
    FileHsmObjectStoreClientConfig config;
    config.deserialize(config_data);
    do_initialize(id, cache_path, config);
}

void FileHsmObjectStoreClient::do_initialize(
    const std::string& id,
    const std::string& cache_path,
    const FileHsmObjectStoreClientConfig& config)
{
    m_id = id;

    m_store = config.m_root.get_value();

    if (m_store.is_relative()) {
        m_store = std::filesystem::path(cache_path) / m_store;
    }

    LOG_INFO("Initializing with root: " + m_store.string());

    if (!std::filesystem::exists(m_store)) {
        std::filesystem::create_directories(m_store);
    }
}

std::filesystem::path FileHsmObjectStoreClient::get_tier_path(
    uint8_t tier) const
{
    return m_store / ("tier" + std::to_string(tier));
}

void FileHsmObjectStoreClient::put(
    const HsmObjectStoreRequest& request, Stream* stream) const
{
    FileObjectStoreClient md_client;
    md_client.do_initialize(
        m_id, m_store / "metadata", FileObjectStoreClient::Mode::METADATA_ONLY);
    if (const auto response = md_client.make_request(
            HsmObjectStoreRequest::to_base_request(request), stream);
        !response->ok()) {
        const std::string msg =
            "Error in file client PUT: " + response->get_error().to_string();
        throw RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }

    if (stream == nullptr) {
        return;
    }

    FileObjectStoreClient data_client;
    data_client.do_initialize(
        m_id, get_tier_path(request.target_tier()),
        FileObjectStoreClient::Mode::DATA_ONLY);
    if (const auto response = data_client.make_request(
            HsmObjectStoreRequest::to_base_request(request), stream);
        !response->ok()) {
        const std::string msg =
            "Error in file client PUT: " + response->get_error().to_string();
        throw RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
}

void FileHsmObjectStoreClient::get(
    const HsmObjectStoreRequest& request,
    StorageObject& object,
    Stream* stream) const
{
    LOG_INFO("Getting metadata");
    FileObjectStoreClient md_client;
    md_client.do_initialize(
        m_id, m_store / "metadata", FileObjectStoreClient::Mode::METADATA_ONLY);
    if (const auto response = md_client.make_request(
            HsmObjectStoreRequest::to_base_request(request), stream);
        !response->ok()) {
        const std::string msg = "Error in file client metadata GET: "
                                + response->get_error().to_string();
        throw RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
    else {
        object = response->object();
    }

    if (stream == nullptr) {
        return;
    }

    LOG_INFO("Getting data");
    FileObjectStoreClient data_client;
    data_client.do_initialize(
        m_id, get_tier_path(request.source_tier()),
        FileObjectStoreClient::Mode::DATA_ONLY);
    if (const auto response = data_client.make_request(
            HsmObjectStoreRequest::to_base_request(request), stream);
        !response->ok()) {
        const std::string msg = "Error in file client data GET: "
                                + response->get_error().to_string();
        throw RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
}

void FileHsmObjectStoreClient::remove(
    const HsmObjectStoreRequest& request) const
{
    FileObjectStoreClient md_client;
    md_client.do_initialize(
        m_id, get_tier_path(request.source_tier()),
        FileObjectStoreClient::Mode::METADATA_ONLY);
    if (const auto response = md_client.make_request(
            HsmObjectStoreRequest::to_base_request(request));
        !response->ok()) {
        const std::string msg =
            "Error in file client REMOVE: " + response->get_error().to_string();
        throw RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }

    FileObjectStoreClient data_client;
    data_client.do_initialize(
        m_id, get_tier_path(request.source_tier()),
        FileObjectStoreClient::Mode::DATA_ONLY);
    if (const auto response = data_client.make_request(
            HsmObjectStoreRequest::to_base_request(request));
        !response->ok()) {
        const std::string msg =
            "Error in file client REMOVE: " + response->get_error().to_string();
        throw RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
}

void FileHsmObjectStoreClient::copy(const HsmObjectStoreRequest& request) const
{
    FileObjectStoreClient file_client;
    file_client.do_initialize(
        m_id, get_tier_path(request.source_tier()),
        FileObjectStoreClient::Mode::DATA_ONLY);
    file_client.migrate(
        request.object().id(), get_tier_path(request.target_tier()), true);
}

void FileHsmObjectStoreClient::move(const HsmObjectStoreRequest& request) const
{
    FileObjectStoreClient file_client;
    file_client.do_initialize(
        m_id, get_tier_path(request.source_tier()),
        FileObjectStoreClient::Mode::DATA_ONLY);
    file_client.migrate(
        request.object().id(), get_tier_path(request.target_tier()), false);
}
}  // namespace hestia
