#include "MockMotrTypes.h"

#include "CompositeLayer.h"

#include <algorithm>

namespace mock::motr {

void Layout::add_layer(std::unique_ptr<CompositeLayer> layer)
{
    m_layers.insert(std::move(layer));
}

void Layout::delete_layer(Id sub_obj_id)
{
    auto pred = [sub_obj_id](const std::unique_ptr<CompositeLayer>& layer) {
        return layer->m_id == sub_obj_id;
    };
    if (auto iter = std::find_if(m_layers.begin(), m_layers.end(), pred);
        iter != m_layers.end()) {
        m_layers.erase(iter);
    }
}

CompositeLayer* Layout::get_top_layer() const
{
    if (m_layers.empty()) {
        return nullptr;
    }
    return m_layers.begin()->get();
}

CompositeLayer* Layout::get_layer(Id sub_object_id) const
{
    auto pred = [sub_object_id](const std::unique_ptr<CompositeLayer>& layer) {
        return layer->m_id == sub_object_id;
    };
    if (auto iter = std::find_if(m_layers.begin(), m_layers.end(), pred);
        iter != m_layers.end()) {
        return iter->get();
    }
    return nullptr;
}

std::optional<PoolVersion> Client::get_pool_version(ostk::Uuid id)
{
    for (const auto& pool : m_pools) {
        if (pool->id() == id) {
            return pool->id();
        }
    }
    return std::nullopt;
}

void Client::add_pool(ostk::Uuid pool_version)
{
    auto pool = std::make_unique<ostk::BlockStore>(pool_version);
    m_pools.push_back(std::move(pool));
}

ostk::BlockStore* Client::get_pool(ostk::Uuid pool_version) const
{
    for (const auto& pool : m_pools) {
        if (pool->id() == pool_version) {
            return pool.get();
        }
    }
    return nullptr;
}

ostk::BlockStore* Client::get_default_pool() const
{
    if (m_pools.empty()) {
        return nullptr;
    }
    return m_pools[0].get();
}
}  // namespace mock::motr