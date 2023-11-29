#pragma once

#include "Dictionary.h"
#include "EventSink.h"
#include "Map.h"

#include <string>

namespace hestia {

class HsmService;
class HsmObject;

class HsmEventSink : public EventSink {
  public:
    HsmEventSink(const std::string& output_file, HsmService* hsm_service);

    void on_event(const CrudEvent& event) override;

  private:
    void on_object_create(const CrudEvent& event) const;

    void on_object_update(const CrudEvent& event) const;

    void on_object_create(
        Dictionary& dict,
        const std::string& id,
        const CrudUserContext& user_context) const;

    void on_object_update(
        Dictionary& dict,
        const std::string& id,
        const CrudUserContext& user_context,
        std::time_t update_time) const;

    void on_object_create_or_update(
        Dictionary& dict,
        const HsmObject& object,
        const CrudUserContext& user_context) const;

    void on_object_read(const CrudEvent& event) const;

    void on_object_remove(const CrudEvent& event) const;

    void on_object_remove(Dictionary& dict, const std::string& id) const;

    void write(const std::string& content) const;

    HsmService* m_hsm_service{nullptr};
    std::string m_output_file;
};
}  // namespace hestia