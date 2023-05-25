#pragma once

#include "IMotrInterfaceImpl.h"
#include "MockMotrBackend.h"

namespace hestia::mock::motr {
class Motr {
  public:
    int m0_client_init(Client* client, MotrConfig* config, bool init_motr);

    static int m0_client_fini(Client* client, bool fini_motr);

    int m0_container_init(
        Container* container, Realm* parent, Id* id, Client* client);

    Layout* m0_client_layout_alloc(Layout::Type layout_type);

    static int m0_composite_layer_add(
        Layout* layout, Obj* sub_obj, int priority);

    static int m0_obj_init(Obj* obj, Realm* parent, Id* id, uint64_t layout_id);

    static int m0_entity_open(Obj* obj);

    static int m0_entity_create(hestia::Uuid* pool, Obj* obj);

    static int m0_entity_delete(Obj* obj);

    static uint64_t m0_client_layout_id(Client* instance);

    static Client* m0__obj_instance(const Obj* obj);

    MotrBackend* backend() { return &m_backend; }

  private:
    Realm* get_realm();

    Client* m_client{nullptr};
    Realm m_default_realm;
    Realm* m_realm{nullptr};

    MotrBackend m_backend;
};
}  // namespace hestia::mock::motr