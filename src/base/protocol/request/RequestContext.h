#pragma once

#include "AuthorizationContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ReadableBufferView.h"
#include "Stream.h"

#include <atomic>
#include <functional>

namespace hestia {
class RequestContext {
  public:
    RequestContext();

    [[nodiscard]] StreamState clear_stream();

    void flush_stream();

    bool finished() const;

    Stream* get_stream() const;

    AuthorizationContext& get_auth_context();

    HttpResponse* get_response() const;

    const HttpRequest& get_request() const;

    HttpRequest& get_writeable_request();

    void set_request(const HttpRequest& req);

    void on_input_complete();

    void on_output_complete();

    std::size_t get_chunk_size() const { return m_chunk_size; }

    using onChunkFunc = std::function<std::size_t(
        const ReadableBufferView& buffer, bool finished)>;
    void set_output_chunk_handler(onChunkFunc func);

    using onInputCompleteFunc = std::function<HttpResponse::Ptr()>;
    void set_input_complete_handler(onInputCompleteFunc func);

    using onCompleteFunc = std::function<void(const HttpResponse* response)>;
    void set_output_complete_handler(onCompleteFunc func);

    void set_response(HttpResponse::Ptr http_response);

    [[nodiscard]] IOResult write_to_stream(const ReadableBufferView& buffer);

  private:
    std::size_t m_chunk_size{4000};
    onInputCompleteFunc m_on_input_complete;
    onCompleteFunc m_on_output_complete;
    onChunkFunc m_on_output_chunk;

    HttpRequest m_request;
    HttpResponse::Ptr m_response;
    AuthorizationContext m_auth_context;

    Stream::Ptr m_stream;

    std::atomic<bool> m_finished{false};
};
}  // namespace hestia