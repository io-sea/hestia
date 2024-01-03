#include "FileHsmObjectStoreClient.h"

#include "ErrorUtils.h"
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

FileHsmObjectStoreClient::~FileHsmObjectStoreClient() {}

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
        for (const auto& tier_id : m_tier_ids) {
            FileObjectStoreClientConfig file_config;
            file_config.m_mode.init_value(
                FileObjectStoreClientConfig::Mode::DATA_ONLY);
            file_config.m_root.init_value(
                (m_store / ("tier" + tier_id)).string());

            auto tier_client = std::make_unique<FileObjectStoreClient>();
            tier_client->do_initialize(id, {}, file_config);

            m_tier_clients[tier_id] = std::move(tier_client);
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

FileObjectStoreClient* FileHsmObjectStoreClient::get_client(
    const std::string& tier_id) const
{
    if (auto iter = m_tier_clients.find(tier_id);
        iter != m_tier_clients.end()) {
        return iter->second.get();
    }
    throw std::runtime_error("Unexpected tier input in file hsm client");
}

void FileHsmObjectStoreClient::make_object_store_request(
    HsmObjectStoreContext& ctx, ObjectStoreRequestMethod method) const
{
    if (m_metadata_client) {
        auto object_store_completion_func =
            [ctx, method](ObjectStoreResponse::Ptr response) {
                if (!response->ok()) {
                    const std::string msg =
                        SOURCE_LOC() + " | Error in file client "
                        + ObjectStoreRequest::to_string(method) + ": "
                        + response->get_error().to_string();
                    throw RequestException<HsmObjectStoreError>(
                        {HsmObjectStoreErrorCode::ERROR, msg});
                }
                ctx.m_completion_func(HsmObjectStoreResponse::create(
                    ctx.m_request, std::move(response)));
            };

        ObjectStoreContext object_ctx(
            HsmObjectStoreRequest::to_base_request(ctx.m_request),
            object_store_completion_func, nullptr, nullptr);
        m_metadata_client->make_request(object_ctx);
    }

    bool missing_stream =
        method != ObjectStoreRequestMethod::REMOVE && !ctx.has_stream();
    if (missing_stream || m_tier_clients.empty()) {
        ctx.m_completion_func(
            HsmObjectStoreResponse::create(ctx.m_request, m_id));
        return;
    }

    auto tier_client = get_client(
        method == ObjectStoreRequestMethod::PUT ? ctx.m_request.target_tier() :
                                                  ctx.m_request.source_tier());
    auto object_store_completion_func = [ctx, method](
                                            ObjectStoreResponse::Ptr response) {
        if (!response->ok()) {
            const std::string msg = "Error in file client "
                                    + ObjectStoreRequest::to_string(method)
                                    + ": " + response->get_error().to_string();
            throw RequestException<HsmObjectStoreError>(
                {HsmObjectStoreErrorCode::ERROR, msg});
        }
        ctx.m_completion_func(
            HsmObjectStoreResponse::create(ctx.m_request, std::move(response)));
    };

    auto object_store_progress_func = [ctx](ObjectStoreResponse::Ptr response) {
        ctx.m_progress_func(
            HsmObjectStoreResponse::create(ctx.m_request, std::move(response)));
    };

    ObjectStoreContext object_ctx(
        HsmObjectStoreRequest::to_base_request(ctx.m_request),
        object_store_completion_func, object_store_progress_func, ctx.m_stream);
    tier_client->make_request(object_ctx);
}

void FileHsmObjectStoreClient::put(HsmObjectStoreContext& ctx) const
{
    make_object_store_request(ctx, ObjectStoreRequestMethod::PUT);
}

void FileHsmObjectStoreClient::get(HsmObjectStoreContext& ctx) const
{
    make_object_store_request(ctx, ObjectStoreRequestMethod::GET);
}

void FileHsmObjectStoreClient::remove(HsmObjectStoreContext& ctx) const
{
    make_object_store_request(ctx, ObjectStoreRequestMethod::REMOVE);
}

std::filesystem::path FileHsmObjectStoreClient::get_tier_path(
    const std::string& tier_id) const
{
    return m_store / ("tier" + tier_id);
}

void FileHsmObjectStoreClient::copy(HsmObjectStoreContext& ctx) const
{
    if (m_tier_clients.empty()) {
        return;
    }

    auto source_client = get_client(ctx.m_request.source_tier());
    source_client->migrate(
        ctx.m_request.object().id(), get_tier_path(ctx.m_request.target_tier()),
        true);

    ctx.m_completion_func(HsmObjectStoreResponse::create(ctx.m_request, m_id));
}

void FileHsmObjectStoreClient::move(HsmObjectStoreContext& ctx) const
{
    if (m_tier_clients.empty()) {
        return;
    }

    auto source_client = get_client(ctx.m_request.source_tier());
    source_client->migrate(
        ctx.m_request.object().id(), get_tier_path(ctx.m_request.target_tier()),
        false);

    ctx.m_completion_func(HsmObjectStoreResponse::create(ctx.m_request, m_id));
}
}  // namespace hestia
