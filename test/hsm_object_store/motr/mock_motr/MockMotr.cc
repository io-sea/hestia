#include "MockMotr.h"

namespace hestia::mock::motr {

int Motr::m0_composite_layer_add(Layout* layout, Obj* sub_obj, int priority)
{
    auto layer         = std::make_unique<CompositeLayer>();
    layer->m_priority  = priority;
    layer->m_id        = sub_obj->m_id;
    layer->m_parent_id = sub_obj->m_layout_id;

    layout->add_layer(std::move(layer));
    return 0;
}

Layout* Motr::m0_client_layout_alloc(Layout::Type layout_type)
{
    return m_backend.allocate_layout(layout_type);
}

int Motr::m0_client_init(Client* client, MotrConfig* config, bool init_motr)
{
    client = client;
    backend()->set_client(client);

    backend()->allocate_layout(Layout::Type::DEFAULT);
    return 0;
}

int Motr::m0_container_init(
    Container* container, Realm* parent, Id* id, Client* client)
{
    container->m_co_realm.m_client = client;
    m_realm                        = &(container->m_co_realm);
    return 0;
}

int Motr::m0_client_fini(Client* client, bool fini_motr)
{
    client->m_is_initialized = false;
    return 0;
}

int Motr::m0_obj_init(Obj* obj, Realm* parent, Id* id, uint64_t layout_id)
{
    obj->m_id        = *id;
    obj->m_realm     = parent;
    obj->m_layout_id = layout_id;
    return 0;
}

int Motr::m0_entity_open(Obj* obj)
{
    auto backend_obj = MotrBackend::get_object(obj->m_realm, obj->m_id);
    if (backend_obj == nullptr) {
        return -1;
    }
    obj->clone(backend_obj);
    return 0;
}

int Motr::m0_entity_delete(Obj* obj)
{
    MotrBackend::delete_object(obj->m_realm, obj->m_id);
    return 0;
}

int Motr::m0_entity_create(hestia::Uuid* pool, Obj* obj)
{
    if (pool != nullptr) {
        obj->m_pool = *pool;
    }
    MotrBackend::add_object(obj);
    return 0;
}

uint64_t Motr::m0_client_layout_id(Client* instance)
{
    return 1;
}

Realm* Motr::get_realm()
{
    if (m_realm != nullptr) {
        return m_realm;
    }
    else {
        return &m_default_realm;
    }
}

Client* Motr::m0__obj_instance(const Obj* obj)
{
    return obj->m_realm->m_client;
}

}  // namespace hestia::mock::motr