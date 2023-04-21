#include "HsmObjectAdapter.h"

HsmObjectAdapter::Ptr HsmObjectAdapter::create()
{
    return std::unique_ptr<HsmObjectAdapter>();
}

void HsmObjectAdapter::serialize(HsmObject& hsm_object)
{
    auto md = &hsm_object.object().mMetadata;
    addIfNotEmpty(
        md, LAST_MODIFIED_KEY,
        std::to_string(hsm_object.object().mLastModifiedTime));
    addIfNotEmpty(
        md, CREATED_KEY, std::to_string(hsm_object.object().mCreationTime));
    addIfNotEmpty(md, DATASET_KEY, hsm_object.dataset());

    std::stringstream sstr;
    for (std::size_t idx = 0; idx < hsm_object.tiers().size() - 1; idx++) {
        sstr << hsm_object.tiers()[idx] << ",";
    }
    if (!hsm_object.tiers().empty()) {
        sstr << hsm_object.tiers()[hsm_object.tiers().size() - 1];
    }
    addIfNotEmpty(md, TIERS_KEY, sstr.str());
}

void HsmObjectAdapter::parse_tiers(HsmObject& hsm_object) {}

void HsmObjectAdapter::sync(
    const HsmObject& hsm_object, ostk::StorageObject& storage_object)
{
}

void HsmObjectAdapter::add_if_not_empty(
    ostk::Metadata* md, const std::string& key, const std::string& value)
{
    if (!value.empty()) {
        md->setItem(key, value);
    }
}