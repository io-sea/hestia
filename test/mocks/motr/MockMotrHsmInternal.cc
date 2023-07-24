#include "MockMotrHsmInternal.h"

#include "Logger.h"

namespace hestia::mock::motr {
uint32_t HsmInternal::hsm_priority(uint32_t generation, uint8_t tier_idx)
{
    uint32_t gen_prio = 0x00FFFFFF - generation;
    return (gen_prio << 8) | tier_idx;
}

Id HsmInternal::hsm_subobj_id(Id id, uint32_t gen, uint8_t tier)
{
    auto hi = id.hi();
    hi <<= 32;
    hi |= (1LL << 31);
    hi |= hsm_priority(gen, tier);
    return {id.lo(), hi};
}

uint32_t HsmInternal::hsm_prio2gen(uint32_t priority)
{
    return 0x00FFFFFF - (priority >> 8);
}

uint8_t HsmInternal::hsm_prio2tier(uint32_t priority)
{
    return priority & 0xFF;
}

hestia::Uuid* HsmInternal::hsm_tier2pool(uint8_t tier_idx) const
{
    if (m_hsm_pools.size() < tier_idx) {
        return nullptr;
    }

    if (tier_idx < 1 || tier_idx > 16) {
        return nullptr;
    }
    return &m_hsm_pools[tier_idx - 1];
}

int HsmInternal::create_obj(
    Id id, Obj* obj, bool close_entity, uint8_t tier_idx) const
{
    (void)close_entity;

    Motr::m0_obj_init(
        obj, m_uber_realm, &id, Motr::m0_client_layout_id(m_client));

    hestia::Uuid* pool{nullptr};
    if (tier_idx != hsm_any_tier) {
        pool = hsm_tier2pool(tier_idx);
        if (pool == nullptr) {
            LOG_ERROR(
                "Failed to find pool for tier: " << std::to_string(tier_idx)
                                                 << ". Pool size is: "
                                                 << m_hsm_pools.size());
            return -1;
        }
    }
    Motr::m0_entity_create(pool, obj);
    return 0;
}

void HsmInternal::layout_top_prio(
    Layout* layout, int32_t* max_gen, Id* max_gen_id, uint8_t* top_tier)
{
    *max_gen  = -1;
    *top_tier = hsm_any_tier;
    for (const auto& layer : layout->m_layers) {
        auto gen = hsm_prio2gen(layer->m_priority);
        // int32_t max_gen_cp = *max_gen;
        if (static_cast<int>(gen) > *max_gen) {
            *max_gen    = gen;
            *max_gen_id = layer->m_id;
        }

        auto tier = hsm_prio2tier(layer->m_priority);
        if (top_tier != nullptr && tier < *top_tier) {
            *top_tier = tier;
        }
    }
}

CompositeLayer* HsmInternal::layer_get_by_prio(Layout* layout, int prio) const
{
    for (const auto& layer : layout->m_layers) {
        if (layer->m_priority == static_cast<uint32_t>(prio)) {
            return layer.get();
        }
    }
    return nullptr;
}

int HsmInternal::check_top_layer_writable(
    Layout* layout, int max_priority, int tier)
{
    auto top_layer = layout->get_top_layer();
    if (top_layer == nullptr) {
        return -2;
    }

    if (top_layer->m_priority >= static_cast<uint32_t>(max_priority)) {
        return -2;
    }

    if (tier != hsm_any_tier && hsm_prio2tier(top_layer->m_priority) != tier) {
        return -2;
    }
    return 0;
}

int HsmInternal::layout_get(Id id, Layout** layout) const
{
    Obj obj;
    Motr::m0_obj_init(
        &obj, m_uber_realm, &id, Motr::m0_client_layout_id(m_client));

    motr()->m0_entity_open(&obj);

    auto rc = obj_layout_get(&obj, layout);
    return rc;
}

int HsmInternal::obj_layout_get(Obj* obj, Layout** layout) const
{
    auto obj_layout = motr()->backend()->get_layout(obj->m_layout_id);
    if (obj_layout == nullptr) {
        return -1;
    }
    *layout = obj_layout;
    return 0;
}

int HsmInternal::layout_set(Id id, Layout* layout) const
{
    Obj obj;
    Motr::m0_obj_init(
        &obj, m_uber_realm, &id, Motr::m0_client_layout_id(m_client));

    Motr::m0_entity_open(&obj);

    obj_layout_set(&obj, layout);
    return 0;
}

int HsmInternal::obj_layout_set(Obj* obj, Layout* layout) const
{
    motr()->backend()->get_object(obj->m_realm, obj->m_id)->m_layout_id =
        layout->m_id;
    obj->m_layout_id = layout->m_id;
    return 0;
}

int HsmInternal::top_layer_add_read_extent(
    Obj* obj, const hestia::Extent& extent) const
{
    Layout* layout{nullptr};
    obj_layout_get(obj, &layout);

    if (layout == nullptr) {
        return -1;
    }

    auto layer = layout->get_top_layer();
    if (layer == nullptr) {
        return -1;
    }
    layer->add_merge_read_extent(extent);

    int rc = 0;
    if (auto gen = hsm_prio2gen(layer->m_priority); gen > 1) {
        /* clean older versions of data in the write tier */
        rc = m0hsm_release_maxgen(
            obj->m_id, hsm_prio2tier(layer->m_priority), gen - 1,
            extent.m_offset, extent.m_length, hsm_rls_flags::HSM_KEEP_LATEST,
            false);
    }
    return rc;
}

int HsmInternal::layer_extent_add(
    Id subobjid, const hestia::Extent& ext, bool write, bool overwrite) const
{
    auto layer = motr()->backend()->get_layer(subobjid);
    if (layer == nullptr) {
        return -1;
    }
    return layer->add_extent(ext, write, overwrite);
}

int HsmInternal::layer_extent_del(Id subobjid, off_t offset, bool write) const
{
    auto layer = motr()->backend()->get_layer(subobjid);
    if (layer == nullptr) {
        return -1;
    }

    hestia::Extent ext;
    ext.m_offset = offset;
    layer->mark_for_deletion(ext, write);
    layer->delete_marked_extents(write);
    return 0;
}

int HsmInternal::delete_obj_set_parent_layout(
    Id id, Id parent_id, Layout* parent_layout) const
{
    Obj obj;
    Motr::m0_obj_init(
        &obj, m_uber_realm, &id, Motr::m0_client_layout_id(m_client));

    motr()->m0_entity_open(&obj);

    motr()->m0_entity_delete(&obj);

    motr()->backend()->set_layout(m_uber_realm, parent_id, parent_layout);
    return 0;
}

int HsmInternal::layer_clean(
    Id parent_id, Layout* layout, CompositeLayer* layer) const
{
    auto sub_obj_id = layer->m_id;
    layout->delete_layer(sub_obj_id);

    auto rc = delete_obj_set_parent_layout(sub_obj_id, parent_id, layout);
    return rc;
}

int HsmInternal::layer_check_clean(
    Id parent_id, Layout* layout, CompositeLayer* layer) const
{
    if (layer->has_read_extents()) {
        return -ENOTEMPTY;
    }
    return layer_clean(parent_id, layout, layer);
}

int HsmInternal::layout_layer_clean(
    Id parent_id, Layout* layout, Id subobj_id) const
{
    for (auto& layer : layout->m_layers) {
        if (!(layer->m_id == subobj_id)) {
            continue;
        }
        return layer_check_clean(parent_id, layout, layer.get());
    }
    return -ENOENT;
}

int HsmInternal::layout_add_top_layer(Id id, Layout* layout, uint8_t tier) const
{
    // Highest generation - lowest tier in this layout
    Id old_id;
    int32_t gen{-1};
    uint8_t top_tier{hsm_any_tier};
    layout_top_prio(layout, &gen, &old_id, &top_tier);

    gen++;

    Obj subobj;
    if (auto rc =
            create_obj(hsm_subobj_id(id, gen, tier), &subobj, false, tier);
        rc) {
        return rc;
    }

    const auto priority = HsmInternal::hsm_priority(gen, tier);
    Motr::m0_composite_layer_add(layout, &subobj, priority);

    const auto full_ext = hestia::Extent::get_full_range_extent();
    if (auto rc = layer_extent_add(subobj.m_id, full_ext, true, false); rc) {
        return rc;
    }

    if (auto rc = layer_extent_del(old_id, 0, true); rc) {
        return rc;
    }

    auto rc = layout_layer_clean(id, layout, old_id);
    if (rc != 0 && rc != -ENOENT && rc != -ENOTEMPTY) {
        return rc;
    }

    if (rc != 0) {
        rc = layout_set(id, layout);
    }
    return rc;
}

int HsmInternal::match_layer_foreach(
    Layout* layout,
    uint8_t tier,
    hestia::Extent* ext,
    layer_match_callback cb,
    void* cb_arg,
    bool stop_on_error)

{
    bool stop = false;
    hestia::Extent match;
    int rc = 0;

    for (auto& layer : layout->m_layers) {
        auto layer_tier = hsm_prio2tier(layer->m_priority);
        if (tier != hsm_any_tier && layer_tier != tier) {
            continue;
        }

        ExtentMatchCode m;
        hestia::Extent current_extents = *ext;
        do {
            m = layer->match_extent(
                current_extents, &match, ExtentMatchType::EMT_INTERSECT, false,
                false);
            if (m == ExtentMatchCode::EM_NONE) {
                continue;
            }

            rc = cb(cb_arg, layout, layer.get(), &match, &stop);
            if ((rc != 0) && stop_on_error) {
                return rc;
            }

            if (m == ExtentMatchCode::EM_PARTIAL) {
                current_extents.m_offset = match.m_offset + match.m_length;
                current_extents.m_length -= match.m_length;
            }

        } while (!stop && m == ExtentMatchCode::EM_PARTIAL);

        if (stop) {
            break;
        }
    }
    return 0;
}

struct min_gen_check_arg {
    int min_gen;
    Id except_subobj;
    hestia::Extent* orig_extent;
    bool found;
};

int HsmInternal::check_min_gen_exists(
    Layout* layout, hestia::Extent* ext, int gen, CompositeLayer* except_layer)
{
    min_gen_check_arg cb_arg;
    cb_arg.min_gen       = gen;
    cb_arg.except_subobj = except_layer->m_id;
    cb_arg.orig_extent   = ext;
    cb_arg.found         = false;

    auto min_gen_check_cb = [](void* cb_arg, Layout* layout,
                               CompositeLayer* layer, hestia::Extent* match,
                               bool* stop) -> int {
        (void)layout;
        (void)match;
        min_gen_check_arg* arg = static_cast<min_gen_check_arg*>(cb_arg);

        if (layer->m_id == arg->except_subobj) {
            return 0;
        }

        if (hsm_prio2gen(layer->m_priority)
            < static_cast<uint32_t>(arg->min_gen)) {
            return 0;
        }

        hestia::Extent dummy;
        if (layer->match_extent(
                *arg->orig_extent, &dummy, ExtentMatchType::EMT_INTERSECT,
                false, false)
            == ExtentMatchCode::EM_FULL) {
            arg->found = true;
            *stop      = true;
            return 0;
        }
        return 0;
    };

    auto rc = match_layer_foreach(
        layout, hsm_any_tier, ext, min_gen_check_cb, &cb_arg, false);
    if (rc != 0 || !cb_arg.found) {
        return -EPERM;
    }
    return 0;
}

void HsmInternal::print_layout(
    std::string& sink, Layout* layout, bool details) const
{
    if (details) {
        sink += std::to_string(layout->m_layers.size()) + " layers:\n";
    }

    std::size_t count = 0;
    for (const auto& layer : layout->m_layers) {
        if (details) {
            sink += "=== layer " + std::to_string(count) + " ===\n";
        }
        else {
            sink += " - ";
        }
        print_layer(sink, layer.get(), details);
        count++;
    }
}

void HsmInternal::print_layer(
    std::string& sink, CompositeLayer* layer, bool details) const
{
    auto gen  = hsm_prio2gen(layer->m_priority);
    auto tier = hsm_prio2tier(layer->m_priority);

    if (details) {
        sink += "subobj=" + layer->m_id.to_string() + "\n";
        sink += "lid=" + layer->m_parent_id.to_string() + "\n";
        sink += "priority=" + std::to_string(layer->m_priority);
        sink += " (gen=" + std::to_string(gen)
                + ", tier=" + std::to_string(tier) + ")\n";
    }
    else {
        sink += "gen " + std::to_string(gen) + ", tier " + std::to_string(tier)
                + ", extents:";
    }

    if (details) {
        sink += " R extents:\n";
    }
    sink += layer->dump_extents(details, false);

    if (details) {
        sink += " W extents:\n";
        sink += layer->dump_extents(details, true);
    }
    else {
        sink += layer->has_write_extents() ? " (writable)\n" : "\n";
    }
}

int HsmInternal::m0hsm_release_maxgen(
    Id id,
    uint8_t tier,
    int max_gen,
    off_t offset,
    size_t len,
    hsm_rls_flags flags,
    bool user_display) const
{
    (void)flags;
    (void)user_display;

    Layout* layout{nullptr};
    if (auto rc = layout_get(id, &layout); rc) {
        return rc;
    }

    hestia::Extent ext;
    ext.m_offset = offset;
    ext.m_length = len;

    ReleaseContext ctx;
    ctx.obj_id   = id;
    ctx.tier     = tier;
    ctx.max_gen  = max_gen;
    ctx.max_tier = tier;

    auto on_layer_matched = [this](
                                void* cb_arg, Layout* layout,
                                CompositeLayer* layer, hestia::Extent* match,
                                bool* stop) {
        auto ctx = static_cast<ReleaseContext*>(cb_arg);
        return this->on_layer_match_for_release(
            ctx, layout, layer, match, stop);
    };
    auto rc =
        match_layer_foreach(layout, tier, &ext, on_layer_matched, &ctx, false);

    return rc;
}

int HsmInternal::on_layer_match_for_release(
    ReleaseContext* ctx,
    Layout* layout,
    CompositeLayer* layer,
    hestia::Extent* match,
    bool* stop) const
{
    (void)stop;

    auto gen = HsmInternal::hsm_prio2gen(layer->m_priority);
    if (ctx->max_gen != -1) {
        if (gen > static_cast<uint32_t>(ctx->max_gen)) {
            return 0;
        }
    }

    if (ctx->max_tier != -1) {
        auto tier = HsmInternal::hsm_prio2tier(layer->m_priority);
        if (tier > ctx->max_tier) {
            return 0;
        }
    }
    ctx->found++;

    if (auto rc = HsmInternal::check_min_gen_exists(layout, match, gen, layer);
        rc) {
        return rc;
    }

    auto rc = HsmInternal::check_top_layer_writable(
        layout, layer->m_priority, hsm_any_tier);
    if (rc == -ENOENT) {
        if (rc = layout_add_top_layer(ctx->obj_id, layout, ctx->tier);
            rc != 0) {
            return rc;
        }
    }

    bool layer_empty = false;
    if (rc = layer->extent_substract(*match, false, &layer_empty); rc != 0) {
        return rc;
    }

    if (layer_empty) {
        if (rc = layer_clean(ctx->obj_id, layout, layer); rc != 0) {
            return rc;
        }
    }

    return 0;
}

int HsmInternal::prepare_io_ctx(
    IoContext& ctx, int num_blocks, size_t block_size, bool alloc_io_buff)
{
    ctx.m_data.clear();
    ctx.m_extents.clear();
    ctx.m_stage.clear();

    if (alloc_io_buff) {
        ctx.m_stage = std::vector<char>(num_blocks * block_size);
        for (int idx = 0; idx < num_blocks; idx++) {
            ctx.m_data.m_buffers.push_back(
                ctx.m_stage.data() + idx * block_size);
        }
    }

    return 0;
}

int HsmInternal::map_io_ctx(
    IoContext& ctx, int num_blocks, size_t b_size, off_t offset, char* buff)
{
    for (int idx = 0; idx < num_blocks; idx++) {
        ctx.m_extents.m_indices.push_back(offset + idx * b_size);
        ctx.m_extents.m_counts.push_back(b_size);

        ctx.m_data.m_counts.push_back(b_size);
        if (buff != nullptr) {
            ctx.m_data.m_buffers.push_back(buff + idx * b_size);
        }
    }
    return 0;
}

int HsmInternal::write_blocks(const Obj& obj, const IoContext& ctx) const
{
    return motr()->backend()->write_object(obj, ctx.m_extents, ctx.m_data);
}

int HsmInternal::read_blocks(const Obj& obj, const IoContext& ctx) const
{
    return motr()->backend()->read_object(obj, ctx.m_extents, ctx.m_data);
}

int HsmInternal::copy_extent_data(
    Id src_id, Id tgt_id, hestia::Extent* range) const
{
    Obj src_obj;
    Obj tgt_obj;
    Motr::m0_obj_init(
        &src_obj, m_uber_realm, &src_id, Motr::m0_client_layout_id(m_client));
    Motr::m0_obj_init(
        &tgt_obj, m_uber_realm, &tgt_id, Motr::m0_client_layout_id(m_client));

    Motr::m0_entity_open(&src_obj);
    Motr::m0_entity_open(&tgt_obj);

    std::intmax_t rest = range->m_length;
    auto start         = range->m_offset;
    auto block_size    = m_io_block_size;

    IoContext ctx;
    for (; rest > 0; rest -= block_size, start += block_size) {
        if (rest < static_cast<std::intmax_t>(block_size)) {
            block_size = rest;
        }

        if (auto rc = prepare_io_ctx(ctx, 1, block_size, true); rc) {
            return rc;
        }

        if (auto rc = map_io_ctx(ctx, 1, block_size, start, nullptr); rc) {
            return rc;
        }

        if (auto rc = read_blocks(src_obj, ctx); rc) {
            return rc;
        }

        if (auto rc = write_blocks(tgt_obj, ctx); rc) {
            return rc;
        }
    }
    return 0;
}

}  // namespace hestia::mock::motr