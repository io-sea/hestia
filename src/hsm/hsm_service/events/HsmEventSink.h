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
    void on_user_metadata_update(
        const CrudUserContext& user_context,
        Dictionary& dict,
        const std::string& id,
        const Map& metadata);

    void on_user_metadata_read(
        const CrudUserContext& user_context,
        Dictionary& dict,
        const std::string& id);

    HsmService* m_hsm_service{nullptr};
    std::string m_output_file;
};
}  // namespace hestia