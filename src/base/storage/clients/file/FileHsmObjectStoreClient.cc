#include "FileHsmObjectStoreClient.h"

#include "FileObjectStoreClient.h"
#include "FileUtils.h"
#include "RequestException.h"

#include <stdexcept>

#include "Logger.h"

namespace hestia {

FileHsmObjectStoreClientConfig::FileHsmObjectStoreClientConfig() :
    SerializeableWithFields("file_hsm_object_store_client_config")
{
    init();
}

FileHsmObjectStoreClientConfig::FileHsmObjectStoreClientConfig(
    const FileHsmObjectStoreClientConfig& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

FileHsmObjectStoreClientConfig& FileHsmObjectStoreClientConfig::operator=(
    const FileHsmObjectStoreClientConfig& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_root = other.m_root;
        m_mode = other.m_mode;
        init();
    }
    return *this;
}

void FileHsmObjectStoreClientConfig::init()
{
    register_scalar_field(&m_root);
    register_scalar_field(&m_mode);
}

const std::string& FileHsmObjectStoreClientConfig::get_root() const
{
    return m_root.get_value();
}

FileObjectStoreClientConfig::Mode FileHsmObjectStoreClientConfig::get_mode()
    const
{
    return m_mode.get_value();
}

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

    m_store = config.get_root();

    if (m_store.is_relative()) {
        m_store = std::filesystem::path(cache_path) / m_store;
    }

    LOG_INFO("Initializing with root: " + m_store.string());

    if (!std::filesystem::exists(m_store)) {
        std::filesystem::create_directories(m_store);
    }

    if (config.get_mode() != FileObjectStoreClientConfig::Mode::METADATA_ONLY) {
        for (const auto& tier_id : m_tier_names) {
            FileObjectStoreClientConfig file_config;
            file_config.m_mode.init_value(
                FileObjectStoreClientConfig::Mode::DATA_ONLY);
            file_config.m_root.init_value(
                (m_store / ("tier" + tier_id)).string());

            auto tier_client = std::make_unique<FileObjectStoreClient>();
            tier_client->do_initialize(id, {}, file_config);

            m_tier_clients[std::stoul(tier_id)] = std::move(tier_client);
        }
    }

    if (config.get_mode() != FileObjectStoreClientConfig::Mode::DATA_ONLY) {
        FileObjectStoreClientConfig file_config;
        file_config.m_mode.init_value(
            FileObjectStoreClientConfig::Mode::METADATA_ONLY);
        file_config.m_root = (m_store / "metadata").string();
        m_metadata_client  = std::make_unique<FileObjectStoreClient>();
    }
}

FileObjectStoreClient* FileHsmObjectStoreClient::get_client(uint8_t tier) const
{
    if (auto iter = m_tier_clients.find(tier); iter != m_tier_clients.end()) {
        return iter->second.get();
    }
    throw std::runtime_error("Unexpected tier input in file hsm client");
}

void FileHsmObjectStoreClient::put(
    const HsmObjectStoreRequest& request, Stream* stream) const
{
    if (m_metadata_client) {
        if (const auto response = m_metadata_client->make_request(
                HsmObjectStoreRequest::to_base_request(request), stream);
            !response->ok()) {
            const std::string msg = "Error in file client PUT: "
                                    + response->get_error().to_string();
            throw RequestException<HsmObjectStoreError>(
                {HsmObjectStoreErrorCode::ERROR, msg});
        }
    }

    if (stream == nullptr || m_tier_clients.empty()) {
        return;
    }

    auto tier_client = get_client(request.target_tier());
    if (const auto response = tier_client->make_request(
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
    if (m_metadata_client) {
        LOG_INFO("Getting metadata");

        if (const auto response = m_metadata_client->make_request(
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
    }

    if (stream == nullptr || m_tier_clients.empty()) {
        return;
    }

    LOG_INFO("Getting data for tier: " << request.source_tier());
    auto tier_client = get_client(request.source_tier());
    if (const auto response = tier_client->make_request(
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
    if (m_metadata_client) {
        if (const auto response = m_metadata_client->make_request(
                HsmObjectStoreRequest::to_base_request(request));
            !response->ok()) {
            const std::string msg = "Error in file client REMOVE: "
                                    + response->get_error().to_string();
            throw RequestException<HsmObjectStoreError>(
                {HsmObjectStoreErrorCode::ERROR, msg});
        }
    }

    if (m_tier_clients.empty()) {
        return;
    }

    auto tier_client = get_client(request.source_tier());
    if (const auto response = tier_client->make_request(
            HsmObjectStoreRequest::to_base_request(request));
        !response->ok()) {
        const std::string msg =
            "Error in file client REMOVE: " + response->get_error().to_string();
        throw RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
}

std::filesystem::path FileHsmObjectStoreClient::get_tier_path(
    uint8_t tier) const
{
    return m_store / ("tier" + std::to_string(tier));
}

void FileHsmObjectStoreClient::copy(const HsmObjectStoreRequest& request) const
{
    if (m_tier_clients.empty()) {
        return;
    }

    auto source_client = get_client(request.source_tier());
    source_client->migrate(
        request.object().id(), get_tier_path(request.target_tier()), true);
}

void FileHsmObjectStoreClient::move(const HsmObjectStoreRequest& request) const
{
    if (m_tier_clients.empty()) {
        return;
    }

    auto source_client = get_client(request.source_tier());
    source_client->migrate(
        request.object().id(), get_tier_path(request.target_tier()), false);
}
}  // namespace hestia
