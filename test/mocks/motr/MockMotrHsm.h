#pragma once

#include "MockMotr.h"
#include "MockMotrHsmInternal.h"

#include <string>

namespace hestia::mock::motr {

struct HsmOptions {
    std::vector<hestia::Uuid> m_pool_fids;
};

class Hsm {
  public:
    int m0hsm_init(Client* client, Realm* realm, HsmOptions* options) const;

    int m0hsm_create(Id id, Obj* obj, int tier, bool keep_open) const;

    int m0hsm_pwrite(
        Obj* obj, void* buf, std::size_t len, std::size_t offset) const;

    int m0hsm_set_write_tier(Id id, uint8_t tier_idx) const;

    enum hsm_cp_flags {
        HSM_MOVE          = (1 << 0),
        HSM_KEEP_OLD_VERS = (1 << 1),
        HSM_WRITE_TO_DEST = (1 << 2)
    };
    int m0hsm_copy(
        Id obj_id,
        uint8_t src_tier,
        uint8_t target_tier,
        off_t offset,
        size_t length,
        hsm_cp_flags flags) const;

    int m0hsm_release(
        Id obj_id,
        uint8_t tier_idx,
        off_t offset,
        size_t length,
        hsm_rls_flags flags) const;

    int m0hsm_stage(
        Id obj_id,
        uint8_t target_tier,
        off_t offset,
        size_t length,
        hsm_cp_flags flags) const;

    int m0hsm_archive(
        Id obj_id,
        uint8_t target_tier,
        off_t offset,
        size_t length,
        hsm_cp_flags flags) const;

    int m0hsm_multi_release(
        Id obj_id,
        uint8_t max_tier,
        off_t offset,
        size_t length,
        hsm_rls_flags flags) const;

    int m0hsm_dump(std::string& sink, Id obj_id, bool details) const;

    // Below not in Motr provided API but needed for mock or real impl
    int m0hsm_set_read_tier(Id obj_id, uint8_t tier_idx) const;

    int m0hsm_read(
        Id obj_id, void* buf, std::size_t len, std::size_t offset) const;

    const Motr* motr() const { return m_impl.motr(); }

    const HsmInternal* impl() const { return &m_impl; }

  private:
    struct CopyContext {
        int found{0};
        Id obj_id;
        uint8_t src_tier{0};
        uint8_t tgt_tier{0};
        Hsm::hsm_cp_flags flags;
    };

    int on_layer_match_for_copy(
        CopyContext* ctx,
        Layout* layout,
        CompositeLayer* src_layer,
        hestia::Extent* match,
        bool* stop) const;
    int on_layer_match_for_stage(
        CopyContext* ctx,
        Layout* layout,
        CompositeLayer* src_layer,
        hestia::Extent* match,
        bool* stop) const;
    int on_layer_match_for_archive(
        CopyContext* ctx,
        Layout* layout,
        CompositeLayer* src_layer,
        hestia::Extent* match,
        bool* stop) const;

    int on_release_post_copy(
        CopyContext* ctx,
        Layout* layout,
        CompositeLayer* src_layer,
        hestia::Extent* match,
        int gen) const;

    HsmInternal m_impl;
    static constexpr int max_block_count{200};
    std::size_t m_io_block_size{HsmInternal::default_block_size};
};

}  // namespace hestia::mock::motr