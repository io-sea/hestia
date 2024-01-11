#include "InMemoryHsmObjectStoreClient.h"

#include "RequestException.h"

#include "ErrorUtils.h"
#include "Logger.h"

#include <cassert>
#include <stdexcept>

namespace hestia {

InMemoryObjectStoreClientConfig::InMemoryObjectStoreClientConfig() :
    SerializeableWithFields("in_memory_hsm_object_store_client")
{
    init();
}

InMemoryObjectStoreClientConfig::InMemoryObjectStoreClientConfig(
    const InMemoryObjectStoreClientConfig& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

InMemoryObjectStoreClientConfig& InMemoryObjectStoreClientConfig::operator=(
    const InMemoryObjectStoreClientConfig& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        init();
    }
    return *this;
}

void InMemoryObjectStoreClientConfig::init() {}

InMemoryHsmObjectStoreClient::InMemoryHsmObjectStoreClient()
{
    LOG_INFO("Created");
}

InMemoryHsmObjectStoreClient::~InMemoryHsmObjectStoreClient() {}

std::string InMemoryHsmObjectStoreClient::get_registry_identifier()
{
    return "hestia::InMemoryHsmObjectStoreClient";
}

InMemoryHsmObjectStoreClient::Ptr InMemoryHsmObjectStoreClient::create()
{
    return std::make_unique<InMemoryHsmObjectStoreClient>();
}

void InMemoryHsmObjectStoreClient::initialize(
    const std::string& id,
    const std::string& cache_path,
    const Dictionary& config_data)
{
    InMemoryObjectStoreClientConfig config;
    config.deserialize(config_data);
    do_initialize(id, cache_path, config);
}

void InMemoryHsmObjectStoreClient::do_initialize(
    const std::string& id,
    const std::string&,
    const InMemoryObjectStoreClientConfig&)
{
    m_id = id;

    for (const auto& tier_id : m_tier_ids) {
        m_tiers[tier_id] = std::make_unique<InMemoryObjectStoreClient>();
    }
}

InMemoryObjectStoreClient* InMemoryHsmObjectStoreClient::get_tier_client(
    const std::string& tier_id) const
{
    if (auto iter = m_tiers.find(tier_id); iter != m_tiers.end()) {
        return iter->second.get();
    }
    throw std::runtime_error(
        SOURCE_LOC() + " | Client not found for tier " + tier_id
        + " - it is missing from the input config.");
}

std::string InMemoryHsmObjectStoreClient::dump() const
{
    std::string output;
    for (const auto& [name, client] : m_tiers) {
        output += "Tier: " + name + '\n';
        output += client->dump() + '\n';
    }
    return output;
}

void InMemoryHsmObjectStoreClient::make_object_store_request(
    HsmObjectStoreContext& ctx, ObjectStoreRequestMethod method) const
{
    auto tier_client = get_tier_client(
        method == ObjectStoreRequestMethod::PUT ? ctx.m_request.target_tier() :
                                                  ctx.m_request.source_tier());

    auto object_store_completion_func = [ctx, method](
                                            ObjectStoreResponse::Ptr response) {
        if (!response->ok()) {
            const std::string msg = "Error in memory client "
                                    + ObjectStoreRequest::to_string(method)
                                    + ": " + response->get_error().to_string();
            throw RequestException<HsmObjectStoreError>(
                {HsmObjectStoreErrorCode::ERROR, msg});
        }
        ctx.finish(
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

void InMemoryHsmObjectStoreClient::put(HsmObjectStoreContext& ctx) const
{
    make_object_store_request(ctx, ObjectStoreRequestMethod::PUT);
}

void InMemoryHsmObjectStoreClient::get(HsmObjectStoreContext& ctx) const
{
    make_object_store_request(ctx, ObjectStoreRequestMethod::GET);
}

void InMemoryHsmObjectStoreClient::remove(HsmObjectStoreContext& ctx) const
{
    make_object_store_request(ctx, ObjectStoreRequestMethod::REMOVE);
}

void InMemoryHsmObjectStoreClient::copy(HsmObjectStoreContext& ctx) const
{
    auto source_client = get_tier_client(ctx.m_request.source_tier());
    auto target_client = get_tier_client(ctx.m_request.target_tier());
    source_client->migrate(ctx.m_request.object().id(), target_client, false);
    ctx.finish(HsmObjectStoreResponse::create(ctx.m_request, m_id));
}

void InMemoryHsmObjectStoreClient::move(HsmObjectStoreContext& ctx) const
{
    auto source_client = get_tier_client(ctx.m_request.source_tier());
    auto target_client = get_tier_client(ctx.m_request.target_tier());
    source_client->migrate(ctx.m_request.object().id(), target_client, true);
    ctx.finish(HsmObjectStoreResponse::create(ctx.m_request, m_id));
}
}  // namespace hestia
