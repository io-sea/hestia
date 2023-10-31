#include "EventSink.h"

#include "Logger.h"

namespace hestia {

CrudEventFileSink::CrudEventFileSink()
//:
// m_adapter(std::make_unique<ModelSerializer>(std::make_unique<TypedModelFactory<CrudEvent>>))
{
}

void CrudEventFileSink::on_event(const CrudEvent& event)
{
    Dictionary dict;
    m_adapter->to_dict(event, dict);

    std::ofstream output_file;
    output_file.open(m_output_path, std::ios::app);
    output_file << JsonDocument(dict).to_string();
}

CrudEventBufferSink::CrudEventBufferSink()
//  :
// m_adapter(std::make_unique<ModelSerializer>(std::make_unique<TypedModelFactory<CrudEvent>>))
{
}

void CrudEventBufferSink::on_event(const CrudEvent& event)
{
    Dictionary dict;
    m_adapter->to_dict(event, dict);
    m_events.append(JsonDocument(dict).to_string());
}

const std::string& CrudEventBufferSink::get_buffer() const
{
    return m_events;
}

}  // namespace hestia