#pragma once

#include "Dictionary.h"
#include "EventSink.h"
#include "Map.h"

#include <string>

namespace hestia {

class HsmService;

class HsmEventSink : public EventSink {
  public:
    HsmEventSink(const std::string& output_file, HsmService* hsm_service);

    void on_event(const CrudEvent& event) override;

    bool will_handle(
        const std::string& subject_type, CrudMethod method) const override;

  private:
    void on_user_metadata_update(const CrudEvent& event) const;

    void on_user_metadata_update(
        const CrudUserContext& user_context,
        Dictionary& dict,
        const std::string& id,
        const Map& metadata) const;

    void on_user_metadata_read(const CrudEvent& event) const;

    void on_user_metadata_read(
        const CrudUserContext& user_context,
        Dictionary& dict,
        const std::string& id) const;

    void on_extent_changed(const CrudEvent& event) const;

    void on_extent_changed(
        const CrudUserContext& user_context,
        Dictionary& dict,
        const std::string& id) const;

    std::string get_metadata_object_id(
        const CrudUserContext& user_context,
        const std::string& metadata_id) const;

    void on_object_create(const CrudEvent& event) const;

    void on_object_create(
        Dictionary& dict, const std::string& id, const Map& metadata) const;

    void on_object_remove(const CrudEvent& event) const;

    void on_object_remove(Dictionary& dict, const std::string& id) const;

    void write(const std::string& content) const;

    Dictionary* add_root(Dictionary& input) const;

    HsmService* m_hsm_service{nullptr};
    std::string m_output_file;
};
}  // namespace hestia