#include "CurlClient.h"

#include "ErrorUtils.h"
#include "Logger.h"

#include <stdexcept>

namespace hestia {

CurlClient::CurlClient(const CurlClientConfig& config) : m_config(config) {}

CurlClient::~CurlClient()
{
    LOG_INFO("Shutting Down");
    if (m_config.m_do_global_init) {
        curl_global_cleanup();
    }
}

void CurlClient::initialize()
{
    LOG_INFO("Initializing curl");
    if (m_config.m_do_global_init) {
        auto rc = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (rc != 0) {
            throw std::runtime_error("Failed curl global init");
        }
    }
    m_initialized = true;
    LOG_INFO("Curl intialized");
}

size_t CurlClient::curl_write_data(
    void* buffer, size_t, size_t nmemb, void* userp)
{
    LOG_INFO("Callback fired: ");
    if (userp == nullptr) {
        return 0;
    }

    auto client = reinterpret_cast<CurlClient*>(userp);
    return client->on_write(buffer, nmemb);
}

size_t CurlClient::on_write(void* buffer, size_t nmemb)
{
    auto handle        = m_handles[std::this_thread::get_id()];
    size_t num_written = nmemb;

    if (handle->m_request_context.m_stream != nullptr) {
        ReadableBufferView buffer_view(const_cast<void*>(buffer), nmemb);
        auto result = handle->m_request_context.m_stream->write(buffer_view);
        if (!result.ok()) {
            LOG_ERROR("Error populating stream");
        }
        num_written = result.m_num_transferred;
    }
    else {
        auto char_buf = reinterpret_cast<char*>(buffer);
        std::vector<char> body(nmemb, 0);
        for (std::size_t idx = 0; idx < nmemb; idx++) {
            body[idx] = char_buf[idx];
        }
        handle->m_request_context.m_response->append_to_body(
            {body.begin(), body.end()});
    }

    LOG_INFO("Got response with size: " << nmemb);
    return num_written;
}

size_t CurlClient::curl_read_data(
    char* buffer, size_t, size_t nmemb, void* userp)
{
    LOG_INFO("Callback fired: ");
    if (userp == nullptr) {
        return 0;
    }

    auto client = reinterpret_cast<CurlClient*>(userp);
    return client->on_read(buffer, nmemb);
}

size_t CurlClient::on_read(char* buffer, size_t nmemb)
{
    auto handle      = m_handles[std::this_thread::get_id()];
    auto num_to_read = nmemb;
    if (handle->m_request_context.m_stream != nullptr) {
        if (handle->m_request_context.m_stream->has_content()) {
            WriteableBufferView buffer_view(buffer, num_to_read);
            auto result = handle->m_request_context.m_stream->read(buffer_view);
            if (!result.ok()) {
                LOG_ERROR("Error reading from stream");
            }
            return result.m_num_transferred;
        }
        else {
            return 0;
        }
    }
    else {
        if (handle->m_request_context.m_read_offset + nmemb
            > handle->m_request_context.m_request->body().size()) {
            num_to_read = handle->m_request_context.m_request->body().size();
        }
        LOG_INFO("Returning " << num_to_read << " bytes ");
        for (std::size_t idx = 0; idx < num_to_read; idx++) {
            buffer[idx] =
                handle->m_request_context.m_request
                    ->body()[handle->m_request_context.m_read_offset + idx];
        }
        handle->m_request_context.m_read_offset += num_to_read;
        LOG_INFO(
            "New offset is " << handle->m_request_context.m_read_offset
                             << " bytes ");
    }
    return num_to_read;
}

void CurlClient::setup_handle(CurlHandle* handle)
{
    auto rc = curl_easy_setopt(
        handle->m_handle, CURLOPT_WRITEFUNCTION, curl_write_data);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl writefunction with error: "
            + handle->m_error_buffer);
    }

    rc = curl_easy_setopt(handle->m_handle, CURLOPT_WRITEDATA, this);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl writedata with error: "
            + handle->m_error_buffer);
    }

    rc = curl_easy_setopt(
        handle->m_handle, CURLOPT_READFUNCTION, curl_read_data);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl readfunction with error: "
            + handle->m_error_buffer);
    }

    rc = curl_easy_setopt(handle->m_handle, CURLOPT_READDATA, this);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl readdata with error: "
            + handle->m_error_buffer);
    }
}

HttpResponse::Ptr CurlClient::make_request(
    const HttpRequest& request, Stream* stream)
{
    if (!m_initialized) {
        initialize();
    }

    auto handle                           = std::make_unique<CurlHandle>();
    m_handles[std::this_thread::get_id()] = handle.get();

    setup_handle(handle.get());

    if (request.get_method() == HttpRequest::Method::GET) {
        handle->prepare_get();
    }
    else if (request.get_method() == HttpRequest::Method::PUT) {
        handle->prepare_put(request, stream);
    }
    else if (request.get_method() == HttpRequest::Method::PUT) {
        handle->prepare_put(request, stream);
    }

    handle->prepare_headers(request.get_header());

    auto response = HttpResponse::create();

    handle->m_request_context.m_request  = &request;
    handle->m_request_context.m_response = response.get();
    handle->m_request_context.m_stream   = stream;

    const auto url = request.get_path();
    curl_easy_setopt(handle->m_handle, CURLOPT_URL, url.c_str());

    LOG_INFO("Making request to: " << url);
    LOG_INFO(request.to_string());
    auto rc = curl_easy_perform(handle->m_handle);
    if (rc != CURLE_OK) {
        std::string msg = SOURCE_LOC() + " | Failed request: " + url;
        if (!handle->m_error_buffer.empty()) {
            msg += " with error: " + handle->m_error_buffer;
        }
        throw std::runtime_error(msg);
    }

    long http_code = 0;
    curl_easy_getinfo(handle->m_handle, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200 || rc == CURLE_ABORTED_BY_CALLBACK) {
        LOG_INFO("Error in http response: " << http_code);
        response =
            HttpResponse::create(http_code, SOURCE_LOC() + " | Curl Error");
    }

    m_handles.erase(std::this_thread::get_id());

    LOG_INFO("Request all done");

    return response;
}

}  // namespace hestia