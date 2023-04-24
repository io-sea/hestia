#pragma once

#include <ostk/BlockStore.h>

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

class CompositeLayer;

namespace mock::motr {
using Id          = ostk::Uuid;
using PoolVersion = ostk::Uuid;

struct Layout {
    enum class Type { DEFAULT, COMPOSITE };

    CompositeLayer* get_top_layer() const;

    CompositeLayer* get_layer(Id sub_object_id) const;

    void add_layer(std::unique_ptr<CompositeLayer> layer);

    void delete_layer(Id sub_obj_id);

    Id m_id;
    Type m_type{Type::DEFAULT};
    std::set<std::unique_ptr<CompositeLayer>> m_layers;
};

struct IndexVec {
    void clear()
    {
        m_indices.clear();
        m_counts.clear();
    }
    std::vector<std::size_t> m_counts;
    std::vector<std::size_t> m_indices;
};

struct BufferVec {
    void clear()
    {
        m_counts.clear();
        m_buffers.clear();
    }
    std::vector<std::size_t> m_counts;
    std::vector<char*> m_buffers;
};

struct Realm;

struct Obj {
    void clone(Obj* obj)
    {
        m_layout_id    = obj->m_layout_id;
        m_pool         = obj->m_pool;
        m_pool_version = obj->m_pool_version;
    }

    Id m_id;
    Id m_layout_id;

    ostk::Uuid m_pool;
    ostk::Uuid m_pool_version;

    Realm* m_realm{nullptr};
};
using ObjPtr = std::unique_ptr<Obj>;

struct Client;
struct Realm {
    Client* m_client{nullptr};
    std::set<ObjPtr> m_objects;
};

struct Container : public Realm {
    Realm m_co_realm;
};

struct LayoutDomain {
    std::vector<std::unique_ptr<Layout>> m_layouts;
};

struct RequestHandler {
    PoolVersion m_pool_version;
    LayoutDomain m_layout_domain;
};

struct Client {
    std::optional<PoolVersion> get_pool_version(ostk::Uuid id);

    void add_pool(ostk::Uuid pool_version);

    ostk::BlockStore* get_pool(ostk::Uuid pool_version) const;

    ostk::BlockStore* get_default_pool() const;

    RequestHandler m_request_handler;
    std::vector<std::unique_ptr<ostk::BlockStore>> m_pools;
    bool m_is_initialized{false};
    bool m_is_hsm_initialized{false};
};
}  // namespace mock::motr