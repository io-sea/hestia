#include "HsmEventSink.h"

#include "Dictionary.h"
#include "HashUtils.h"
#include "Logger.h"
#include "TimeUtils.h"
#include "YamlUtils.h"

#include "HsmItem.h"
#include "HsmService.h"

#include <cassert>
#include <string>

namespace hestia {

HsmEventSink::HsmEventSink(
    const std::string& output_file, HsmService* hsm_service) :
    m_hsm_service(hsm_service), m_output_file(output_file)
{
    (void)m_hsm_service;
}

void add_scalar(
    Dictionary& dict,
    const std::string& key,
    const std::string& value,
    const std::string& tag         = "",
    const std::string& prefix      = "",
    const std::string& default_val = "")
{
    if (dict.get_type() != Dictionary::Type::MAP) {
        LOG_ERROR("Dictionary must be map to set scalar by key");
        return;
    }

    if (value.empty() && default_val.empty()) {
        return;
    }

    dict.set_map_item(key, Dictionary::create(Dictionary::Type::SCALAR));
    if (value.empty()) {
        dict.get_map_item(key)->set_scalar(default_val);
    }
    else {
        dict.get_map_item(key)->set_scalar(value);
    }
    dict.get_map_item(key)->set_tag(tag, prefix);
}

void set_string(
    Dictionary& dict, const std::string& key, const std::string& value)
{
    add_scalar(dict, key, value, "str", "!");
}

void set_xattrs(Dictionary& dict, const Map& meta)
{
    dict.set_map_item("attrs", Dictionary::create(Dictionary::Type::MAP));
    auto xattrs = dict.get_map_item("attrs");

    for (const auto& [key, value] : meta.data()) {
        add_scalar(*xattrs, key, value, "str");
    }
}

void on_object_create(
    Dictionary& dict, const std::string& id, const Map& metadata)
{
    dict.set_tag("create");

    // ID Field
    set_string(dict, "id", id);
    set_string(dict, "time", std::to_string(TimeUtils::get_current_time()));

    // Xattrs
    Map xattrs;
    metadata.copy_with_prefix(
        {"dataset.id", "creation_time", "size"}, xattrs, {}, false);
    set_xattrs(dict, xattrs);
}

void on_object_remove(Dictionary& dict, const std::string& id)
{
    dict.set_tag("remove");
    set_string(dict, "id", id);
    set_string(dict, "time", std::to_string(TimeUtils::get_current_time()));
}

void HsmEventSink::on_user_metadata_update(
    const CrudUserContext& user_context,
    Dictionary& dict,
    const std::string& id,
    const Map& metadata)
{
    auto metadata_service = m_hsm_service->get_service(HsmItem::Type::METADATA);

    CrudQuery query(CrudIdentifier(id), CrudQuery::OutputFormat::ITEM);
    auto response =
        metadata_service->make_request(CrudRequest{query, user_context});
    if (!response->found()) {
        throw std::runtime_error(
            "Failed to find requested item in event sink check");
    }

    const auto object_id = response->get_item_as<UserMetadata>()->object();

    dict.set_tag("update");
    set_string(dict, "id", object_id);
    set_string(dict, "time", std::to_string(TimeUtils::get_current_time()));

    Map xattrs;
    metadata.copy_with_prefix({"data."}, xattrs, "user_metadata.");
    set_xattrs(dict, xattrs);
}

void HsmEventSink::on_user_metadata_read(
    const CrudUserContext& user_context,
    Dictionary& dict,
    const std::string& id)
{
    auto metadata_service = m_hsm_service->get_service(HsmItem::Type::METADATA);

    CrudQuery query(CrudIdentifier(id), CrudQuery::OutputFormat::ITEM);
    auto response =
        metadata_service->make_request(CrudRequest{query, user_context, false});
    if (!response->found()) {
        throw std::runtime_error(
            "Failed to find requested item in event sink check");
    }

    const auto object_id = response->get_item_as<UserMetadata>()->object();

    dict.set_tag("read");
    set_string(dict, "id", object_id);
    set_string(dict, "time", std::to_string(TimeUtils::get_current_time()));
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
    return false;
}

void HsmEventSink::on_event(const CrudEvent& event)
{
    bool sorted = false;

    if (event.get_subject_type() == HsmItem::hsm_object_name) {
        if (event.get_method() == CrudMethod::CREATE) {
            LOG_INFO("Got hsm object create");

            std::string out;
            std::size_t count{0};
            for (const auto& id : event.get_ids()) {
                Dictionary output_dict;
                output_dict.set_map_item("root", Dictionary::create());
                auto root = output_dict.get_map_item("root");
                on_object_create(*root, id, event.get_modified_attrs()[count]);
                count++;

                YamlUtils::dict_to_yaml(output_dict, out, sorted);
            }

            std::ofstream output_file;
            output_file.open(m_output_file, std::ios::app);
            output_file << out;
        }
        else if (event.get_method() == CrudMethod::REMOVE) {
            LOG_INFO("Got hsm object remove");

            std::string out;
            for (const auto& id : event.get_ids()) {
                LOG_INFO("Got removing id: " << id);
                Dictionary output_dict;
                output_dict.set_map_item("root", Dictionary::create());
                auto root = output_dict.get_map_item("root");

                on_object_remove(*root, id);

                YamlUtils::dict_to_yaml(output_dict, out, sorted);
            }

            std::ofstream output_file;
            output_file.open(m_output_file, std::ios::app);
            output_file << out;
        }
    }
    else if (event.get_subject_type() == HsmItem::user_metadata_name) {
        if (event.get_method() == CrudMethod::UPDATE) {
            std::string out;
            std::size_t count{0};
            assert(event.get_ids().size() == event.get_modified_attrs().size());
            for (const auto& id : event.get_ids()) {
                Dictionary output_dict;
                output_dict.set_map_item("root", Dictionary::create());
                auto root = output_dict.get_map_item("root");
                on_user_metadata_update(
                    event.get_user_context(), *root, id,
                    event.get_modified_attrs()[count]);
                YamlUtils::dict_to_yaml(output_dict, out, sorted);
                count++;
            }
            std::ofstream output_file;
            output_file.open(m_output_file, std::ios::app);
            output_file << out;
        }
        else if (event.get_method() == CrudMethod::READ) {
            std::string out;
            for (const auto& id : event.get_ids()) {
                Dictionary output_dict;
                output_dict.set_map_item("root", Dictionary::create());
                auto root = output_dict.get_map_item("root");
                on_user_metadata_read(event.get_user_context(), *root, id);
                YamlUtils::dict_to_yaml(output_dict, out, sorted);
            }
            std::ofstream output_file;
            output_file.open(m_output_file, std::ios::app);
            output_file << out;
        }
    }
}

}  // namespace hestia