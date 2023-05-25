#pragma once

#include "CompositeLayer.h"
#include "MockMotrTypes.h"

namespace hestia::mock::motr {
class MotrBackend {
  public:
    static void add_object(Obj* object);

    static void delete_object(Realm* realm, Id id);

    static Obj* get_object(Realm* realm, Id id);

    enum class IoType { READ, WRITE };

    int read_object(
        const Obj& obj, const IndexVec& extents, const BufferVec& data);

    int write_object(
        const Obj& obj, const IndexVec& extents, const BufferVec& data);

    Layout* allocate_layout(Layout::Type layout_type);

    Layout* get_layout(Id id);

    CompositeLayer* get_layer(Id id);

    static void set_layout(Realm* realm, Id obj_id, Layout* layout);

    void set_client(Client* client);

  private:
    int do_object_io(
        const Obj& obj,
        const IndexVec& extents,
        const BufferVec& data,
        IoType io_type);

    int do_composite_io(
        const Obj& obj,
        const IndexVec& extents,
        const BufferVec& data,
        IoType io_type);

    static hestia::Uuid obj_id_to_fid(Id id);
    Client* m_client{nullptr};
};
}  // namespace hestia::mock::motr