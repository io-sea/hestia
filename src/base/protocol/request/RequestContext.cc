#include "RequestContext.h"

#include "Logger.h"

namespace hestia {
RequestContext::RequestContext(const HttpRequest& request) :
    m_response(HttpResponse::create()), m_stream(Stream::create())
{
    m_request = request;
    m_request.set_context(this);
}

void RequestContext::set_request(const HttpRequest& request)
{
    m_request = request;
    m_request.set_context(this);
}

Stream* RequestContext::get_stream() const
{
    return m_stream.get();
}

AuthorizationContext* RequestContext::get_auth_context()
{
    return &m_auth_context;
}

HttpResponse* RequestContext::get_response() const
{
    return m_response.get();
}

const HttpRequest& RequestContext::get_request() const
{
    return m_request;
}

void RequestContext::set_output_chunk_handler(onChunkFunc func)
{
    m_on_output_chunk = func;
}

void RequestContext::set_input_complete_handler(onInputCompleteFunc func)
{
    m_on_input_complete = func;
}

void RequestContext::set_output_complete_handler(onCompleteFunc func)
{
    m_on_output_complete = func;
}

void RequestContext::set_response(HttpResponse::Ptr http_response)
{
    m_response = std::move(http_response);
}

void RequestContext::on_output_complete()
{
    m_finished = true;
}

StreamState RequestContext::clear_stream()
{
    return m_stream->reset();
}

bool RequestContext::finished() const
{
    return m_finished;
}

void RequestContext::flush_stream()
{
    if (!m_on_output_chunk) {
        return;
    }

    std::vector<char> buffer(m_chunk_size);
    WriteableBufferView writeable_buffer(&buffer[0], m_chunk_size);
    while (true) {
        const auto result = m_stream->read(writeable_buffer);
        if (!result.ok()) {
            m_response = HttpResponse::create(500, "Internal Server Error");
            break;
        }

        ReadableBufferView readable_buffer(
            &buffer[0], result.m_num_transferred);
        m_on_output_chunk(readable_buffer, result.finished());
        if (result.finished()) {
            break;
        }
    }

    if (const auto stream_state = m_stream->reset(); !stream_state.ok()) {
        m_response = HttpResponse::create(500, "Internal Server Error");
    }

    if (m_on_output_complete) {
        m_on_output_complete(m_response.get());
    }

    on_output_complete();
}

IOResult RequestContext::write_to_stream(const ReadableBufferView& buffer)
{
    return m_stream->write(buffer);
}

void RequestContext::on_input_complete()
{
    if (const auto stream_state = m_stream->reset(); !stream_state.ok()) {
        m_response = HttpResponse::create(500, "Internal Server Error");
    }
    else {
        if (m_on_input_complete) {
            LOG_INFO("Calling Input Complete Handler.");
            m_response = m_on_input_complete();
        }
        else if (!m_response) {
            m_response = HttpResponse::create();
        }
        m_finished = true;
    }
}
}  // namespace hestia