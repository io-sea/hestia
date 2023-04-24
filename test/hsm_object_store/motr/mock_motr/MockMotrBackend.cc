#include "MockMotrBackend.h"

#include "MockMotr.h"

namespace mock::motr {
void MotrBackend::add_object(Obj* object)
{
    auto obj_ptr            = std::make_unique<Obj>(*object);
    obj_ptr->m_pool_version = obj_ptr->m_pool;
    object->m_realm->m_objects.insert(std::move(obj_ptr));
}

Obj* MotrBackend::get_object(Realm* realm, Id id)
{
    auto pred = [id](const std::unique_ptr<Obj>& obj) {
        return obj->m_id == id;
    };
    auto objs = &realm->m_objects;
    if (auto iter = std::find_if(objs->begin(), objs->end(), pred);
        iter != objs->end()) {
        return iter->get();
    }
    return nullptr;
}

void MotrBackend::delete_object(Realm* realm, Id id)
{
    auto objects = &(realm->m_objects);
    auto pred    = [id](const ObjPtr& obj) { return obj->m_id == id; };
    if (auto iter = std::find_if(objects->begin(), objects->end(), pred);
        iter != objects->end()) {
        objects->erase(iter);
    }
}

struct CompositeSubObjectExtents {
    int m_offset{0};
    int m_length{0};
    void* m_buffer{nullptr};
};

struct CompositeSubObject {
    Id m_id;
    uint64_t m_layout_id{0};
    std::vector<CompositeSubObjectExtents> m_extents;
};

struct ExtentCursor {
    bool at_end() const { return m_iter == m_extents->end(); }

    ostk::Extent* item() const { return &(m_iter->second); }

    void next() { m_iter++; }

    void set_extents(CompositeLayer::ExtentList* extents)
    {
        m_extents = extents;
        m_iter    = m_extents->begin();
    }

    CompositeLayer::ExtentList::iterator m_iter;
    CompositeLayer::ExtentList* m_extents{nullptr};
};

static void advance_layer_cursors(
    std::vector<ExtentCursor>& cursors, std::size_t offset)
{
    // Move through extents until they end after the offset
    for (auto& cursor : cursors) {
        if (cursor.at_end() || cursor.item()->get_end() > offset) {
            continue;
        }
        cursor.next();
        while (!cursor.at_end() && cursor.item()->get_end() <= offset) {
            cursor.next();
        }
    }
}

static std::size_t get_next_offset(
    const std::vector<ExtentCursor>& cursors, int start_idx)
{
    int layer_idx{0};
    auto next_offset = cursors[start_idx].item()->get_end();
    for (layer_idx = start_idx - 1; layer_idx >= 0; layer_idx--) {
        if (cursors[layer_idx].at_end()) {
            continue;
        }
        if (cursors[layer_idx].item()->m_offset <= next_offset) {
            break;
        }
    }
    if (layer_idx >= 0) {
        next_offset = cursors[layer_idx].item()->m_offset;
    }
    return next_offset;
}

static void build_layer_data(
    Layout* layout,
    std::vector<CompositeSubObject>& sub_objects,
    std::vector<ExtentCursor>& extent_cursors,
    MotrBackend::IoType io_type)
{
    for (const auto& layer : layout->m_layers) {
        auto extents =
            layer->get_extents(io_type == MotrBackend::IoType::WRITE);
        if (extents->empty()) {
            continue;
        }

        ExtentCursor cursor;
        cursor.set_extents(extents);
        extent_cursors.push_back(cursor);

        CompositeSubObject sub_object;
        sub_object.m_id        = layer->m_id;
        sub_object.m_layout_id = layer->m_parent_id.m_lo;
        sub_objects.push_back(sub_object);
    }
}

static int build_layer_extents(
    const IndexVec& input_extents,
    const BufferVec& data,
    std::vector<CompositeSubObject>& sub_objects,
    std::vector<ExtentCursor>& extent_cursors)
{
    auto index_cursor = 0;
    auto offset       = input_extents.m_indices[index_cursor];
    auto length{0};
    advance_layer_cursors(extent_cursors, offset);

    while (index_cursor < input_extents.m_indices.size()) {
        offset = input_extents.m_indices[index_cursor];
        length = input_extents.m_counts[index_cursor];

        std::size_t valid_layer_idx{0};
        for (const auto& cursor : extent_cursors) {
            if (!cursor.at_end() && offset >= cursor.item()->m_offset) {
                break;
            }
            valid_layer_idx++;
        }

        auto next_offset = get_next_offset(extent_cursors, valid_layer_idx);
        if (next_offset > offset + length) {
            next_offset = offset + length;
        }
        length = next_offset - offset;

        CompositeSubObjectExtents subobject_extents;
        subobject_extents.m_offset = offset;
        subobject_extents.m_length = length;
        subobject_extents.m_buffer = data.m_buffers[index_cursor];
        sub_objects[valid_layer_idx].m_extents.push_back(subobject_extents);

        advance_layer_cursors(extent_cursors, next_offset);
        index_cursor++;
    }
    return 0;
}

int MotrBackend::do_composite_io(
    const Obj& obj,
    const IndexVec& input_extents,
    const BufferVec& data,
    IoType io_type)
{
    auto layout = get_layout(obj.m_layout_id);

    std::vector<CompositeSubObject> sub_objects;
    std::vector<ExtentCursor> extent_cursors;

    build_layer_data(layout, sub_objects, extent_cursors, io_type);

    auto rc =
        build_layer_extents(input_extents, data, sub_objects, extent_cursors);
    if (rc != 0) {
        return rc;
    }

    for (const auto& sub_object : sub_objects) {
        if (sub_object.m_extents.empty()) {
            continue;
        }

        IndexVec sub_extents;
        BufferVec sub_data;
        for (const auto& sub_object_extent : sub_object.m_extents) {
            sub_extents.m_counts.push_back(sub_object_extent.m_length);
            sub_extents.m_indices.push_back(sub_object_extent.m_offset);

            sub_data.m_counts.push_back(sub_object_extent.m_length);
            auto buf = reinterpret_cast<char*>(sub_object_extent.m_buffer);
            if (io_type == IoType::WRITE) {
                std::string str_buf(buf, buf + 3);
            }
            sub_data.m_buffers.push_back(
                reinterpret_cast<char*>(sub_object_extent.m_buffer));
        }

        auto sub_obj = get_object(obj.m_realm, sub_object.m_id);
        do_object_io(*sub_obj, sub_extents, sub_data, io_type);
    }
    return 0;
}

int MotrBackend::read_object(
    const Obj& obj, const IndexVec& ext, const BufferVec& data)
{
    return do_object_io(obj, ext, data, IoType::READ);
}

int MotrBackend::write_object(
    const Obj& obj, const IndexVec& ext, const BufferVec& data)
{
    return do_object_io(obj, ext, data, IoType::WRITE);
}

int MotrBackend::do_object_io(
    const Obj& obj,
    const IndexVec& extents,
    const BufferVec& data,
    IoType io_type)
{
    auto layout = get_layout(obj.m_layout_id);
    if (layout->m_type == Layout::Type::COMPOSITE) {
        return do_composite_io(obj, extents, data, io_type);
    }

    auto client = Motr::m0__obj_instance(&obj);
    auto pool   = client->get_pool(obj.m_pool_version);
    if (pool == nullptr) {
        pool = client->get_default_pool();
    }
    if (pool == nullptr) {
        return -1;
    }

    for (std::size_t idx = 0; idx < extents.m_indices.size(); idx++) {
        auto offset  = extents.m_indices[idx];
        auto length  = extents.m_counts[idx];
        auto buf_loc = data.m_buffers[idx];
        if (io_type == IoType::READ) {
            ostk::WriteableBufferView buffer(buf_loc, length);
            pool->read(obj.m_id.to_string(), {offset, length}, buffer);
        }
        else {
            ostk::ReadableBufferView buffer(buf_loc);
            pool->write(obj.m_id.to_string(), {offset, length}, buffer);
        }
    }
    return 0;
}

ostk::Uuid MotrBackend::obj_id_to_fid(Id id)
{
    return ostk::Uuid(id.m_lo, id.m_hi);
}

void MotrBackend::set_layout(Realm* realm, Id obj_id, Layout* layout)
{
    auto obj         = get_object(realm, obj_id);
    obj->m_layout_id = layout->m_id;
}

Layout* MotrBackend::allocate_layout(Layout::Type layout_type)
{
    auto layout     = std::make_unique<Layout>();
    auto layout_raw = layout.get();
    layout->m_id =
        m_client->m_request_handler.m_layout_domain.m_layouts.size() + 1;
    layout->m_type = layout_type;
    m_client->m_request_handler.m_layout_domain.m_layouts.push_back(
        std::move(layout));
    return layout_raw;
}

Layout* MotrBackend::get_layout(Id id)
{
    for (auto& layout : m_client->m_request_handler.m_layout_domain.m_layouts) {
        if (layout->m_id == id) {
            return layout.get();
        }
    }
    return nullptr;
}

CompositeLayer* MotrBackend::get_layer(Id subobj_id)
{
    for (auto& layout : m_client->m_request_handler.m_layout_domain.m_layouts) {
        if (auto layer = layout->get_layer(subobj_id); layer) {
            return layer;
        }
    }
    return nullptr;
}

void MotrBackend::set_client(Client* client)
{
    m_client = client;
}
}  // namespace mock::motr