#include "HsmEventSink.h"

#include "Dictionary.h"
#include "HashUtils.h"
#include "Logger.h"
#include "StringUtils.h"
#include "TimeUtils.h"
#include "YamlUtils.h"

#include "HsmItem.h"
#include "HsmService.h"

#include <cassert>
#include <set>
#include <string>

namespace hestia {

HsmEventSink::HsmEventSink(
    const std::string& output_file, HsmService* hsm_service) :
    m_hsm_service(hsm_service), m_output_file(output_file)
{
}

bool HsmEventSink::will_handle(
    const std::string& subject_type, CrudMethod method) const
{
    if (subject_type == HsmItem::hsm_object_name) {
        if (method == CrudMethod::CREATE || method == CrudMethod::REMOVE) {
            return true;
        }
    }
    else if (subject_type == HsmItem::user_metadata_name) {
        if (method == CrudMethod::READ || method == CrudMethod::UPDATE) {
            return true;
        }
    }
    else if (subject_type == HsmItem::tier_extents_name) {
        if (method == CrudMethod::CREATE || method == CrudMethod::UPDATE
            || method == CrudMethod::REMOVE) {
            return true;
        }
    }
    return false;
}

void HsmEventSink::on_event(const CrudEvent& event)
{
    if (event.get_subject_type() == HsmItem::hsm_object_name) {
        if (event.get_method() == CrudMethod::CREATE) {
            on_object_create(event);
        }
        else if (
            event.get_method() == CrudMethod::READ
            && event.get_source() == "HsmService") {
            on_object_read(event);
        }
        else if (event.get_method() == CrudMethod::REMOVE) {
            on_object_remove(event);
        }
    }
    else if (event.get_subject_type() == HsmItem::user_metadata_name) {
        if (event.get_method() == CrudMethod::UPDATE) {
            on_user_metadata_update(event);
        }
        else if (event.get_method() == CrudMethod::READ) {
            on_user_metadata_read(event);
        }
    }
    else if (event.get_subject_type() == HsmItem::tier_extents_name) {
        if (event.get_method() == CrudMethod::CREATE
            || event.get_method() == CrudMethod::UPDATE
            || event.get_method() == CrudMethod::REMOVE) {
            on_extent_changed(event);
        }
    }
}

void HsmEventSink::write(const std::string& content) const
{
    std::ofstream output_file;
    output_file.open(m_output_file, std::ios::app);
    output_file << content;
}

Dictionary* HsmEventSink::add_root(Dictionary& input) const
{
    input.set_map_item("root", Dictionary::create());
    return input.get_map_item("root");
}

void add_scalar(
    Dictionary& dict,
    const std::string& key,
    const std::string& value,
    const std::string& tag = "",
    bool should_quote      = false)
{
    if (value.empty()) {
        return;
    }
    if (dict.get_type() != Dictionary::Type::MAP) {
        LOG_ERROR("Dictionary must be map to set scalar by key");
        return;
    }
    dict.set_map_item(key, Dictionary::create(Dictionary::Type::SCALAR));
    dict.get_map_item(key)->set_scalar(value, should_quote);
    dict.get_map_item(key)->set_tag(tag);
}

void set_string(
    Dictionary& dict, const std::string& key, const std::string& value)
{
    add_scalar(dict, key, value, "", true);
}

void set_literal(
    Dictionary& dict, const std::string& key, const std::string& value)
{
    add_scalar(dict, key, value);
}

void set_xattrs(Dictionary& dict, const Map& meta, const char delim = '.')
{
    dict.set_map_item("attrs", Dictionary::create(Dictionary::Type::MAP));
    auto xattrs = dict.get_map_item("attrs");

    for (const auto& [key, value] : meta.data()) {
        if (value.empty()) {
            continue;
        }
        auto sub_key  = StringUtils::split_on_first(key, delim);
        auto sub_dict = xattrs;
        while (!sub_key.second.empty()) {
            if (!sub_dict->has_map_item(sub_key.first)) {
                sub_dict->set_map_item(
                    sub_key.first, Dictionary::create(Dictionary::Type::MAP));
            }
            sub_dict = sub_dict->get_map_item(sub_key.first);
            sub_key  = StringUtils::split_on_first(sub_key.second, delim);
        }
        add_scalar(*sub_dict, sub_key.first, value, "", true);
    }
}

void HsmEventSink::on_extent_changed(const CrudEvent& event) const
{
    LOG_INFO("Got hsm extent changed");
    std::string out;
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_extent_changed(event.get_user_context(), *add_root(output_dict), id);
        YamlUtils::dict_to_yaml(output_dict, out);
    }
    write(out);
}

void HsmEventSink::on_extent_changed(
    const CrudUserContext& user_context,
    Dictionary& dict,
    const std::string& id) const
{
    const auto extent_service =
        m_hsm_service->get_service(HsmItem::Type::EXTENT);
    const auto response = extent_service->make_request(CrudRequest{
        CrudQuery{CrudIdentifier(id), CrudQuery::OutputFormat::ITEM},
        user_context});
    if (!response->found()) {
        throw std::runtime_error(
            "Failed to find requested item in event sink check");
    }

    const auto extent    = response->get_item_as<TierExtents>();
    const auto object_id = extent->get_object_id();

    const auto object_service =
        m_hsm_service->get_service(HsmItem::Type::OBJECT);
    const auto object_response = object_service->make_request(CrudRequest{
        CrudQuery{CrudIdentifier(object_id), CrudQuery::OutputFormat::ITEM},
        user_context});
    if (!object_response->found()) {
        throw std::runtime_error(
            "Failed to find requested item in event sink check");
    }

    const auto object = object_response->get_item_as<HsmObject>();

    dict.set_tag("update");
    set_string(dict, "id", object_id);
    set_literal(dict, "time", std::to_string(TimeUtils::get_current_time()));

    std::set<std::string> tier_ids;
    for (const auto& extent : object->tiers()) {
        tier_ids.insert(extent.get_tier_id());
    }

    VecCrudIdentifier tier_crud_ids;
    for (const auto& id : tier_ids) {
        tier_crud_ids.push_back(CrudIdentifier(id));
    }

    const auto tier_service  = m_hsm_service->get_service(HsmItem::Type::TIER);
    const auto tier_response = tier_service->make_request(CrudRequest{
        CrudQuery{tier_crud_ids, CrudQuery::OutputFormat::ITEM}, user_context});
    if (!tier_response->found()) {
        throw std::runtime_error(
            "Failed to find requested item in event sink check");
    }

    std::unordered_map<std::string, std::string> tier_names;
    for (const auto& tier : tier_response->items()) {
        tier_names[tier->get_primary_key()] = tier->name();
    }

    Map xattrs;
    for (const auto& tier_extent : object->tiers()) {
        const auto tier_name = tier_names[tier_extent.get_tier_id()];
        assert(!tier_name.empty());
        std::string extents_str;
        for (const auto& [offset, extent] : tier_extent.get_extents()) {
            if (extent.empty()) {
                continue;
            }
            extents_str += std::to_string(extent.m_offset) + ","
                           + std::to_string(extent.m_length) + ";";
        }
        if (!extents_str.empty()) {
            extents_str = extents_str.substr(0, extents_str.size() - 1);
        }
        xattrs.set_item("tiers." + tier_name, extents_str);
    }
    set_xattrs(dict, xattrs);
}

void HsmEventSink::on_object_create(const CrudEvent& event) const
{
    LOG_INFO("Got hsm object create");
    std::string out;
    std::size_t count{0};
    assert(event.get_ids().size() == event.get_modified_attrs().size());
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_object_create(
            *add_root(output_dict), id, event.get_modified_attrs()[count]);
        YamlUtils::dict_to_yaml(output_dict, out);
        count++;
    }
    write(out);
}

void HsmEventSink::on_object_read(const CrudEvent& event) const
{
    LOG_INFO("Object read");
    assert(!event.get_ids().empty());

    Dictionary dict;
    auto root_dict = add_root(dict);

    root_dict->set_tag("read");

    set_string(*root_dict, "id", event.get_ids()[0]);
    set_literal(
        *root_dict, "time", std::to_string(TimeUtils::get_current_time()));

    std::string out;
    YamlUtils::dict_to_yaml(dict, out);
    write(out);
}

void HsmEventSink::on_object_create(
    Dictionary& dict, const std::string& id, const Map& metadata) const
{
    dict.set_tag("create");

    set_string(dict, "id", id);
    set_literal(dict, "time", std::to_string(TimeUtils::get_current_time()));

    Map xattrs;
    metadata.copy_with_prefix({"dataset.id", "name"}, xattrs, {}, false);
    set_xattrs(dict, xattrs);
}

void HsmEventSink::on_object_remove(const CrudEvent& event) const
{
    LOG_INFO("Got hsm object remove");
    std::string out;
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_object_remove(*add_root(output_dict), id);
        YamlUtils::dict_to_yaml(output_dict, out);
    }
    write(out);
}

void HsmEventSink::on_object_remove(
    Dictionary& dict, const std::string& id) const
{
    dict.set_tag("remove");
    set_string(dict, "id", id);
    set_literal(dict, "time", std::to_string(TimeUtils::get_current_time()));
}

std::string HsmEventSink::get_metadata_object_id(
    const CrudUserContext& user_context, const std::string& metadata_id) const
{
    const auto metadata_service =
        m_hsm_service->get_service(HsmItem::Type::METADATA);
    const auto response = metadata_service->make_request(CrudRequest{
        CrudQuery{CrudIdentifier(metadata_id), CrudQuery::OutputFormat::ITEM},
        user_context, false});
    if (!response->found()) {
        throw std::runtime_error(
            "Failed to find requested item in event sink check");
    }
    return response->get_item_as<UserMetadata>()->object();
}

void HsmEventSink::on_user_metadata_update(
    const CrudUserContext& user_context,
    Dictionary& dict,
    const std::string& id,
    const Map& metadata) const
{
    if (metadata.empty()) {
        return;
    }

    const auto object_id = get_metadata_object_id(user_context, id);

    dict.set_tag("update");
    set_string(dict, "id", object_id);
    set_literal(dict, "time", std::to_string(TimeUtils::get_current_time()));

    Map xattrs;
    metadata.copy_with_prefix({"data."}, xattrs, "user_metadata.");
    set_xattrs(dict, xattrs);
}

void HsmEventSink::on_user_metadata_read(
    const CrudUserContext& user_context,
    Dictionary& dict,
    const std::string& id) const
{
    const auto object_id = get_metadata_object_id(user_context, id);

    dict.set_tag("read");
    set_string(dict, "id", object_id);
    set_literal(dict, "time", std::to_string(TimeUtils::get_current_time()));
}

void HsmEventSink::on_user_metadata_update(const CrudEvent& event) const
{
    std::string out;
    std::size_t count{0};
    assert(event.get_ids().size() == event.get_modified_attrs().size());
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_user_metadata_update(
            event.get_user_context(), *add_root(output_dict), id,
            event.get_modified_attrs()[count]);
        YamlUtils::dict_to_yaml(output_dict, out);
        count++;
    }
    write(out);
}

void HsmEventSink::on_user_metadata_read(const CrudEvent& event) const
{
    std::string out;
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_user_metadata_read(
            event.get_user_context(), *add_root(output_dict), id);
        YamlUtils::dict_to_yaml(output_dict, out);
    }
    write(out);
}

}  // namespace hestia