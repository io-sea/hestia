#include "MockMotrHsm.h"

#include <algorithm>
#include <iostream>

#include <cstring>

namespace hestia::mock::motr {

int Hsm::m0hsm_init(Client* client, Realm* realm, HsmOptions* options)
{
    impl()->set_realm(realm);
    impl()->set_pools(options->m_pool_fids);
    impl()->set_client(client);
    return 0;
}

int Hsm::m0hsm_create(Id id, Obj* obj, int tier, bool keep_open)
{
    Obj sub_object;

    auto rc = impl()->create_obj(
        HsmInternal::hsm_subobj_id(id, 0, tier), &sub_object, false, tier);
    if (rc != 0) {
        return rc;
    }

    auto layout = motr()->m0_client_layout_alloc(Layout::Type::COMPOSITE);

    auto priority = HsmInternal::hsm_priority(0, tier);
    Motr::m0_composite_layer_add(layout, &sub_object, priority);

    const auto full_ext = hestia::Extent::get_full_range_extent();
    impl()->layer_extent_add(sub_object.m_id, full_ext, true, false);

    rc = impl()->create_obj(id, obj, false, HsmInternal::hsm_any_tier);

    impl()->layout_set(id, layout);
    obj->m_layout_id = layout->m_id;
    return rc;
}

int Hsm::m0hsm_pwrite(
    Obj* obj, void* buffer, std::size_t length, std::size_t offset)
{
    hestia::Extent write_extent;
    write_extent.m_offset = offset;
    write_extent.m_length = length;

    IoContext ctx;
    char* char_buf = static_cast<char*>(buffer);
    char* pad_buf{nullptr};

    int rc             = 0;
    std::intmax_t rest = length;
    while (rest > 0) {
        int num_blocks = rest / m_io_block_size;
        if (num_blocks == 0) {
            num_blocks = 1;
            pad_buf    = static_cast<char*>(::calloc(1, m_io_block_size));
            if (pad_buf == nullptr) {
                rc = -ENOMEM;
                break;
            }
            ::memcpy(pad_buf, char_buf, rest);
            char_buf = pad_buf;
            write_extent.m_length += m_io_block_size - rest;
            length += m_io_block_size - rest;
        }
        if (num_blocks > max_block_count) {
            num_blocks = max_block_count;
        }

        impl()->prepare_io_ctx(ctx, num_blocks, m_io_block_size, false);
        impl()->map_io_ctx(ctx, num_blocks, m_io_block_size, offset, char_buf);
        rc = impl()->write_blocks(*obj, ctx);
        if (rc != 0) {
            break;
        }
        char_buf += num_blocks * m_io_block_size;
        offset += num_blocks * m_io_block_size;
        rest -= num_blocks * m_io_block_size;
    }
    if (pad_buf != nullptr) {
        ::free(pad_buf);
    }

    if (rc != 0) {
        return rc;
    }

    rc = m_impl.top_layer_add_read_extent(obj, write_extent);
    return rc;
}

int Hsm::m0hsm_set_read_tier(Id obj_id, uint8_t tier_idx)
{
    return 0;
}

int Hsm::m0hsm_read(Id obj_id, void* buf, std::size_t len, std::size_t offset)
{
    Obj obj;
    motr()->m0_obj_init(
        &obj, impl()->get_realm(), &obj_id,
        motr()->m0_client_layout_id(impl()->client()));
    if (auto rc = motr()->m0_entity_open(&obj); rc) {
        return rc;
    }

    std::size_t start = offset;
    auto io_size      = m_io_block_size;
    int num_blocks{0};
    int rc = 0;
    IoContext ctx;
    auto char_buf = reinterpret_cast<char*>(buf);

    for (std::intmax_t rest = len; rest > 0; rest -= num_blocks * io_size) {
        num_blocks = rest / io_size;
        if (num_blocks == 0) {
            num_blocks = 1;
            io_size    = rest;
        }
        if (num_blocks > max_block_count) {
            num_blocks = max_block_count;
        }

        impl()->prepare_io_ctx(ctx, num_blocks, io_size, true);
        impl()->map_io_ctx(ctx, num_blocks, io_size, start, nullptr);
        auto rc = impl()->read_blocks(obj, ctx);

        if (rc != 0) {
            break;
        }

        for (const auto& extent_buffer : ctx.m_data.m_buffers) {
            for (std::size_t byte_idx = 0; byte_idx < io_size; byte_idx++) {
                *char_buf = extent_buffer[byte_idx];
                char_buf++;
            }
        }
        start += num_blocks * io_size;
    }
    return rc;
}

int Hsm::m0hsm_set_write_tier(Id obj_id, uint8_t tier_idx)
{
    Layout* layout{nullptr};
    impl()->layout_get(obj_id, &layout);

    auto top_layer = layout->get_top_layer();
    if (top_layer != nullptr
        && HsmInternal::hsm_prio2tier(top_layer->m_priority) == tier_idx) {
        return 0;
    }
    auto rc = m_impl.layout_add_top_layer(obj_id, layout, tier_idx);
    return rc;
}

int Hsm::m0hsm_copy(
    Id obj_id,
    uint8_t src_tier,
    uint8_t tgt_tier,
    off_t offset,
    std::size_t length,
    hsm_cp_flags flags)
{
    Layout* layout{nullptr};
    if (auto rc = impl()->layout_get(obj_id, &layout); rc != 0) {
        return rc;
    }

    hestia::Extent source_ext;
    source_ext.m_offset = offset;
    source_ext.m_length = length;

    CopyContext ctx;
    ctx.obj_id   = obj_id;
    ctx.src_tier = src_tier;
    ctx.tgt_tier = tgt_tier;
    ctx.flags    = flags;

    auto on_layer_matched = [this](
                                void* cb_arg, Layout* layout,
                                CompositeLayer* src_layer,
                                hestia::Extent* match, bool* stop) {
        auto ctx = static_cast<CopyContext*>(cb_arg);
        return this->on_layer_match_for_copy(
            ctx, layout, src_layer, match, stop);
    };

    auto rc = impl()->match_layer_foreach(
        layout, src_tier, &source_ext, on_layer_matched, &ctx, false);
    return rc;
}

int Hsm::on_layer_match_for_copy(
    CopyContext* ctx,
    Layout* layout,
    CompositeLayer* src_layer,
    hestia::Extent* match,
    bool* stop)
{
    ctx->found++;

    auto gen          = HsmInternal::hsm_prio2gen(src_layer->m_priority);
    auto tgt_priority = HsmInternal::hsm_priority(gen, ctx->tgt_tier);

    auto write_priority = std::min(src_layer->m_priority, tgt_priority);
    uint8_t write_tier{0};

    int rc{0};

    if ((ctx->flags & HSM_WRITE_TO_DEST) != 0) {
        write_tier = ctx->tgt_tier;
        rc         = impl()->check_top_layer_writable(
                    layout, write_priority, write_tier);
    }
    else {
        rc = impl()->check_top_layer_writable(
            layout, write_priority, HsmInternal::hsm_any_tier);
        write_tier = ctx->src_tier;
    }

    if (rc == -ENOENT) {
        if (rc = impl()->layout_add_top_layer(ctx->obj_id, layout, write_tier);
            rc != 0) {
            return rc;
        }
    }

    Obj subobj;
    Id subobj_id;
    auto tgt_layer = impl()->layer_get_by_prio(layout, tgt_priority);
    if (tgt_layer == nullptr) {
        subobj_id = HsmInternal::hsm_subobj_id(ctx->obj_id, gen, ctx->tgt_tier);
        if (rc = impl()->create_obj(subobj_id, &subobj, false, ctx->tgt_tier);
            rc != 0) {
            return rc;
        }
    }
    else {
        hestia::Extent already_ext;

        subobj_id = tgt_layer->m_id;
        auto mc   = tgt_layer->match_extent(
              *match, &already_ext, ExtentMatchType::EMT_INTERSECT, false, false);

        if (mc == ExtentMatchCode::EM_FULL) {
            on_release_post_copy(ctx, layout, src_layer, match, gen);
        }
    }

    if (rc = impl()->copy_extent_data(src_layer->m_id, subobj_id, match);
        rc != 0) {
        return rc;
    }

    if (tgt_layer == nullptr) {
        motr()->m0_composite_layer_add(layout, &subobj, tgt_priority);
        if (rc = impl()->layout_set(ctx->obj_id, layout); rc != 0) {
            return rc;
        }
        if (rc = impl()->layer_extent_add(subobj_id, *match, false, false);
            rc != 0) {
            return rc;
        }
    }
    else {
        if (rc = tgt_layer->add_merge_read_extent(*match); rc != 0) {
            return rc;
        }
    }

    rc = on_release_post_copy(ctx, layout, src_layer, match, gen);
    return rc;
}

int Hsm::on_release_post_copy(
    CopyContext* ctx,
    Layout* layout,
    CompositeLayer* src_layer,
    hestia::Extent* match,
    int gen)
{
    if ((ctx->flags & HSM_MOVE) != 0) {
        HsmInternal::ReleaseContext rls_ctx;
        rls_ctx.obj_id   = ctx->obj_id;
        rls_ctx.tier     = ctx->src_tier;
        rls_ctx.max_gen  = gen;
        rls_ctx.max_tier = ctx->src_tier;

        bool dummy{false};
        if (auto rc = impl()->on_layer_match_for_release(
                &rls_ctx, layout, src_layer, match, &dummy);
            rc) {
            return rc;
        }
    }

    return 0;
    if (((ctx->flags & HSM_KEEP_OLD_VERS) == 0) && gen > 0) {
        auto rc = impl()->m0hsm_release_maxgen(
            ctx->obj_id, ctx->tgt_tier, gen - 1, match->m_offset,
            match->m_length, hsm_rls_flags::HSM_KEEP_LATEST, false);
        if (rc != 0) {
            return rc;
        }
    }
    return 0;
}

int Hsm::m0hsm_release(
    Id obj_id, uint8_t tier, off_t offset, size_t length, hsm_rls_flags flags)
{
    return impl()->m0hsm_release_maxgen(
        obj_id, tier, -1, offset, length, flags, true);
}

int Hsm::on_layer_match_for_stage(
    CopyContext* ctx,
    Layout* layout,
    CompositeLayer* src_layer,
    hestia::Extent* match,
    bool* stop)
{
    const auto tier = HsmInternal::hsm_prio2tier(src_layer->m_priority);
    if (tier <= ctx->tgt_tier) {
        return 0;
    }

    ctx->src_tier = tier;
    auto rc = on_layer_match_for_copy(ctx, layout, src_layer, match, stop);

    return rc;
}

int Hsm::m0hsm_stage(
    Id obj_id,
    uint8_t target_tier,
    off_t offset,
    size_t length,
    hsm_cp_flags flags)
{
    Layout* layout{nullptr};
    if (auto rc = impl()->layout_get(obj_id, &layout); rc) {
        return rc;
    }

    hestia::Extent source_ext;
    source_ext.m_offset = offset;
    source_ext.m_length = length;

    CopyContext ctx;
    ctx.obj_id   = obj_id;
    ctx.src_tier = HsmInternal::hsm_any_tier;
    ctx.tgt_tier = target_tier;
    ctx.flags    = flags;

    auto on_layer_matched = [this](
                                void* cb_arg, Layout* layout,
                                CompositeLayer* src_layer,
                                hestia::Extent* match, bool* stop) {
        auto ctx = static_cast<CopyContext*>(cb_arg);
        return this->on_layer_match_for_stage(
            ctx, layout, src_layer, match, stop);
    };

    auto rc = impl()->match_layer_foreach(
        layout, HsmInternal::hsm_any_tier, &source_ext, on_layer_matched, &ctx,
        false);
    return rc;
}

int Hsm::on_layer_match_for_archive(
    CopyContext* ctx,
    Layout* layout,
    CompositeLayer* src_layer,
    hestia::Extent* match,
    bool* stop)
{
    const auto tier = HsmInternal::hsm_prio2tier(src_layer->m_priority);
    if (tier >= ctx->tgt_tier) {
        return 0;
    }

    ctx->src_tier = tier;
    auto rc = on_layer_match_for_copy(ctx, layout, src_layer, match, stop);

    return rc;
}

int Hsm::m0hsm_archive(
    Id obj_id,
    uint8_t target_tier,
    off_t offset,
    size_t length,
    hsm_cp_flags flags)
{
    Layout* layout{nullptr};
    if (auto rc = impl()->layout_get(obj_id, &layout); rc) {
        return rc;
    }

    hestia::Extent source_ext;
    source_ext.m_offset = offset;
    source_ext.m_length = length;

    CopyContext ctx;
    ctx.obj_id   = obj_id;
    ctx.src_tier = HsmInternal::hsm_any_tier;
    ctx.tgt_tier = target_tier;
    ctx.flags    = flags;

    auto on_layer_matched = [this](
                                void* cb_arg, Layout* layout,
                                CompositeLayer* src_layer,
                                hestia::Extent* match, bool* stop) {
        auto ctx = static_cast<CopyContext*>(cb_arg);
        return this->on_layer_match_for_archive(
            ctx, layout, src_layer, match, stop);
    };

    auto rc = impl()->match_layer_foreach(
        layout, HsmInternal::hsm_any_tier, &source_ext, on_layer_matched, &ctx,
        false);
    return rc;
}

int Hsm::m0hsm_multi_release(
    Id obj_id,
    uint8_t max_tier,
    off_t offset,
    size_t length,
    hsm_rls_flags flags)
{
    Layout* layout{nullptr};
    if (auto rc = impl()->layout_get(obj_id, &layout); rc) {
        return rc;
    }

    hestia::Extent ext;
    ext.m_offset = offset;
    ext.m_length = length;

    HsmInternal::ReleaseContext ctx;
    ctx.obj_id   = obj_id;
    ctx.tier     = HsmInternal::hsm_any_tier;
    ctx.max_gen  = -1;
    ctx.max_tier = max_tier;

    auto on_layer_matched = [this](
                                void* cb_arg, Layout* layout,
                                CompositeLayer* src_layer,
                                hestia::Extent* match, bool* stop) {
        auto ctx = static_cast<HsmInternal::ReleaseContext*>(cb_arg);
        return this->impl()->on_layer_match_for_release(
            ctx, layout, src_layer, match, stop);
    };

    auto rc = impl()->match_layer_foreach(
        layout, HsmInternal::hsm_any_tier, &ext, on_layer_matched, &ctx, false);
    return rc;
}

int Hsm::m0hsm_dump(std::string& sink, Id obj_id, bool details)
{
    Layout* layout{nullptr};
    auto rc = impl()->layout_get(obj_id, &layout);
    if (rc != 0) {
        return rc;
    }
    impl()->print_layout(sink, layout, details);
    return 0;
}

}  // namespace hestia::mock::motr