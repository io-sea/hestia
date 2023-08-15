#pragma once

#include "CrudEvent.h"
#include "StringAdapter.h"

#include <fstream>

namespace hestia {
class EventSink {
  public:
    virtual ~EventSink() = default;

    virtual void on_event(const CrudEvent& event) = 0;

    virtual bool will_handle(const std::string&, CrudMethod) const
    {
        return false;
    }
};

class CrudEventFileSink : public EventSink {
  public:
    CrudEventFileSink();

    void on_event(const CrudEvent& event);

  private:
    std::unique_ptr<ModelFactory> m_model_factory;
    std::unique_ptr<StringAdapter> m_adapter;
    std::string m_output_path{"event_feed.yaml"};
};

class CrudEventBufferSink : public EventSink {
  public:
    CrudEventBufferSink();

    void on_event(const CrudEvent& event) override;

    const std::string& get_buffer() const;

  private:
    std::unique_ptr<ModelFactory> m_model_factory;
    std::unique_ptr<StringAdapter> m_adapter;
    std::string m_events;
};
}  // namespace hestia