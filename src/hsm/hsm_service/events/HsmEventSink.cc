#include "HsmEventSink.h"

#include "Dictionary.h"
#include "ErrorUtils.h"
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

Dictionary* prepare_dict(
    Dictionary& dict,
    const std::string& tag,
    const std::string& id,
    const std::string& time)
{
    auto root_dict = dict.set_map_item("root");
    root_dict->set_tag(tag);
    root_dict->set_map_scalar("id", id, Dictionary::ScalarType::STRING);
    root_dict->set_map_scalar("time", time, Dictionary::ScalarType::INT);
    return root_dict;
}

Dictionary* prepare_dict(
    Dictionary& dict,
    const std::string& tag,
    const std::string& id,
    std::time_t time)
{
    return prepare_dict(dict, tag, id, std::to_string(time));
}

void HsmEventSink::on_extent_changed(const CrudEvent& event) const
{
    LOG_INFO("Got hsm extent changed");
    std::string out;
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_extent_changed(event.get_user_context(), output_dict, id);
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
        CrudMethod::READ,
        CrudQuery{CrudIdentifier(id), CrudQuery::BodyFormat::ITEM},
        user_context, false});
    if (!response->found()) {
        const std::string msg =
            SOURCE_LOC()
            + " | Failed to find requested item in event sink check: " + id;
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }

    const auto extent    = response->get_item_as<TierExtents>();
    const auto object_id = extent->get_object_id();

    on_object_update(dict, object_id, user_context);
}

void HsmEventSink::on_object_create_or_update(
    Dictionary& dict,
    const HsmObject& object,
    const CrudUserContext& user_context) const
{
    auto xattrs = dict.set_map_item("attrs");

    std::size_t object_size{0};
    auto tiers_dict = xattrs->set_map_item("tiers");
    tiers_dict->set_type(Dictionary::Type::SEQUENCE);

    std::vector<std::string> tier_ids;
    for (const auto& extent : object.tiers()) {
        tier_ids.push_back(extent.get_tier_id());
    }

    const auto tier_service  = m_hsm_service->get_service(HsmItem::Type::TIER);
    const auto tier_response = tier_service->make_request(CrudRequest{
        CrudMethod::READ, CrudQuery{tier_ids, CrudQuery::BodyFormat::ITEM},
        user_context, false});
    if (tier_response->found()) {
        std::unordered_map<std::string, std::string> tier_names;
        for (const auto& tier : tier_response->items()) {
            tier_names[tier->get_primary_key()] = tier->name();
        }

        for (const auto& tier_extent : object.tiers()) {
            auto tier_dict = Dictionary::create();

            const auto tier_name = tier_names[tier_extent.get_tier_id()];
            tier_dict->set_map_scalar(
                "name", tier_name, Dictionary::ScalarType::INT);

            auto extents_dict = Dictionary::create(Dictionary::Type::SEQUENCE);
            bool has_extents{false};
            for (const auto& [offset, extent] : tier_extent.get_extents()) {
                if (extent.empty()) {
                    continue;
                }
                has_extents = true;

                auto extent_dict = Dictionary::create();
                extent_dict->set_map_scalar(
                    "offset", std::to_string(extent.m_offset),
                    Dictionary::ScalarType::INT);
                extent_dict->set_map_scalar(
                    "length", std::to_string(extent.m_length),
                    Dictionary::ScalarType::INT);
                const auto extent_max_size = extent.m_offset + extent.m_length;
                if (extent_max_size > object_size) {
                    object_size = extent_max_size;
                }
                extents_dict->add_sequence_item(std::move(extent_dict));
            }
            tier_dict->set_map_item("extents", std::move(extents_dict));
            if (has_extents) {
                tiers_dict->add_sequence_item(std::move(tier_dict));
            }
        }
    }

    xattrs->set_map_scalar(
        "size", std::to_string(object_size), Dictionary::ScalarType::INT);

    Dictionary::FormatSpec dict_format;
    auto metadata_dict = xattrs->set_map_item("user_metadata");
    for (const auto& [key, value] : object.metadata().data()) {
        metadata_dict->set_map_scalar(
            key, value, Dictionary::ScalarType::STRING);
    }
}

void HsmEventSink::on_object_update(
    Dictionary& dict,
    const std::string& object_id,
    const CrudUserContext& user_context) const
{
    const auto object_service =
        m_hsm_service->get_service(HsmItem::Type::OBJECT);
    const auto object_response = object_service->make_request(CrudRequest{
        CrudMethod::READ,
        CrudQuery{CrudIdentifier(object_id), CrudQuery::BodyFormat::ITEM},
        user_context, false});
    if (!object_response->found()) {
        const std::string msg =
            SOURCE_LOC()
            + " | Failed to find requested item in event sink check: "
            + object_id;
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
    const auto object = object_response->get_item_as<HsmObject>();

    auto root_dict = prepare_dict(
        dict, "update", object_id, object->get_last_modified_time());
    on_object_create_or_update(*root_dict, *object, user_context);
}

void HsmEventSink::on_object_create(
    Dictionary& dict,
    const std::string& id,
    const CrudUserContext& user_context) const
{
    LOG_INFO("Object create");

    const auto object_service =
        m_hsm_service->get_service(HsmItem::Type::OBJECT);
    const auto object_response = object_service->make_request(CrudRequest{
        CrudMethod::READ,
        CrudQuery{CrudIdentifier(id), CrudQuery::BodyFormat::ITEM},
        user_context, false});
    if (!object_response->found()) {
        const std::string msg =
            SOURCE_LOC()
            + " | Failed to find requested item in event sink check: " + id;
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
    const auto object = object_response->get_item_as<HsmObject>();

    auto root_dict =
        prepare_dict(dict, "create", id, object->get_creation_time());
    on_object_create_or_update(*root_dict, *object, user_context);
}

void HsmEventSink::on_object_create(const CrudEvent& event) const
{
    LOG_INFO("Got hsm object create");
    std::string out;
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_object_create(output_dict, id, event.get_user_context());
        YamlUtils::dict_to_yaml(output_dict, out);
    }
    write(out);
}

void HsmEventSink::on_object_read(const CrudEvent& event) const
{
    LOG_INFO("Object read");
    assert(!event.get_ids().empty());

    Dictionary dict;
    prepare_dict(
        dict, "read", event.get_ids()[0], TimeUtils::get_current_time());

    std::string out;
    YamlUtils::dict_to_yaml(dict, out);
    write(out);
}

void HsmEventSink::on_object_remove(
    Dictionary& dict, const std::string& id) const
{
    prepare_dict(dict, "remove", id, TimeUtils::get_current_time());
}

void HsmEventSink::on_object_remove(const CrudEvent& event) const
{
    LOG_INFO("Got hsm object remove");
    std::string out;
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_object_remove(output_dict, id);
        YamlUtils::dict_to_yaml(output_dict, out);
    }
    write(out);
}

std::string HsmEventSink::get_metadata_object_id(
    const CrudUserContext& user_context, const std::string& metadata_id) const
{
    const auto metadata_service =
        m_hsm_service->get_service(HsmItem::Type::METADATA);
    const auto response = metadata_service->make_request(CrudRequest{
        CrudMethod::READ,
        CrudQuery{CrudIdentifier(metadata_id), CrudQuery::BodyFormat::ITEM},
        user_context, false});
    if (!response->found()) {
        const std::string msg =
            SOURCE_LOC()
            + " | Failed to find requested item in event sink check: "
            + metadata_id;
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }
    return response->get_item_as<UserMetadata>()->object();
}

void HsmEventSink::on_user_metadata_update(
    const CrudUserContext& user_context,
    Dictionary& dict,
    const std::string& id) const
{
    LOG_INFO("Metadata update");
    const auto object_id = get_metadata_object_id(user_context, id);
    on_object_update(dict, object_id, user_context);
    LOG_INFO("Finished metadata update");
}

void HsmEventSink::on_user_metadata_update(const CrudEvent& event) const
{
    std::string out;
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_user_metadata_update(event.get_user_context(), output_dict, id);
        YamlUtils::dict_to_yaml(output_dict, out);
    }
    write(out);
}

void HsmEventSink::on_object_read(Dictionary& dict, const std::string& id) const
{
    prepare_dict(dict, "read", id, TimeUtils::get_current_time());
}

void HsmEventSink::on_user_metadata_read(
    const CrudUserContext& user_context,
    Dictionary& dict,
    const std::string& id) const
{
    LOG_INFO("Metadata read");
    const auto object_id = get_metadata_object_id(user_context, id);
    on_object_read(dict, object_id);
}

void HsmEventSink::on_user_metadata_read(const CrudEvent& event) const
{
    std::string out;
    for (const auto& id : event.get_ids()) {
        Dictionary output_dict;
        on_user_metadata_read(event.get_user_context(), output_dict, id);
        YamlUtils::dict_to_yaml(output_dict, out);
    }
    write(out);
}

}  // namespace hestia