#include "HsmObjectAdapter.h"

namespace hestia {
HsmObjectAdapter::Ptr HsmObjectAdapter::create()
{
    return std::make_unique<HsmObjectAdapter>();
}

void HsmObjectAdapter::serialize(HsmObject& hsm_object)
{
    auto md = &hsm_object.object().m_metadata;
    add_if_not_empty(
        md, last_modified_key,
        std::to_string(hsm_object.object().m_last_modified_time));
    add_if_not_empty(
        md, created_key, std::to_string(hsm_object.object().m_creation_time));
    add_if_not_empty(md, dataset_key, hsm_object.dataset());

    std::stringstream sstr;
    for (std::size_t idx = 0; idx < hsm_object.tiers().size() - 1; idx++) {
        sstr << hsm_object.tiers()[idx] << ",";
    }
    if (!hsm_object.tiers().empty()) {
        sstr << hsm_object.tiers()[hsm_object.tiers().size() - 1];
    }
    add_if_not_empty(md, tiers_key, sstr.str());
}

void HsmObjectAdapter::parse_tiers(HsmObject& hsm_object)
{
    (void)hsm_object;
}

void HsmObjectAdapter::sync(
    const HsmObject& hsm_object, hestia::StorageObject& storage_object)
{
    (void)hsm_object;
    (void)storage_object;
}

void HsmObjectAdapter::add_if_not_empty(
    hestia::Metadata* md, const std::string& key, const std::string& value)
{
    if (!value.empty()) {
        md->set_item(key, value);
    }
}

void HsmObjectJsonAdapter::to_string(
    const HsmObject& item, std::string& output) const
{
    (void)item;
    (void)output;
}

void HsmObjectJsonAdapter::from_string(
    const std::string& output, HsmObject& item) const
{
    (void)item;
    (void)output;
}

}  // namespace hestia