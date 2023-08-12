#include "EventSink.h"

#include "Logger.h"
#include "StringAdapter.h"

namespace hestia {

CrudEventFileSink::CrudEventFileSink() :
    m_model_factory(std::make_unique<TypedModelFactory<CrudEvent>>()),
    m_adapter(std::make_unique<JsonAdapter>(m_model_factory.get()))
{
}

void CrudEventFileSink::on_event(const CrudEvent& event)
{
    std::string str;
    m_adapter->to_string(event, str);

    std::ofstream output_file;
    output_file.open(m_output_path, std::ios::app);
    output_file << str;
}

CrudEventBufferSink::CrudEventBufferSink() :
    m_model_factory(std::make_unique<TypedModelFactory<CrudEvent>>()),
    m_adapter(std::make_unique<JsonAdapter>(m_model_factory.get()))
{
}

void CrudEventBufferSink::on_event(const CrudEvent& event)
{
    std::string str;
    m_adapter->to_string(event, str);
    m_events.append(str);
}

const std::string& CrudEventBufferSink::get_buffer() const
{
    return m_events;
}

}  // namespace hestia