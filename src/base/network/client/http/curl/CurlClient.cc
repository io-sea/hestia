#include "CurlClient.h"

#include "Logger.h"

#include <stdexcept>

namespace hestia {
CurlClient::CurlClient(const CurlClientConfig& config) : m_config(config) {}

CurlClient::~CurlClient()
{
    LOG_INFO("Shutting Down");
    if (m_handle != nullptr) {
        curl_easy_cleanup(m_handle);
        m_handle = nullptr;
    }

    if (m_config.m_do_global_init) {
        curl_global_cleanup();
    }
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
    auto char_buf = reinterpret_cast<char*>(buffer);
    std::vector<char> body(nmemb);
    for (std::size_t idx = 0; idx < nmemb; idx++) {
        body[idx] = char_buf[idx];
    }

    m_working_response->append_to_body({body.begin(), body.end()});
    LOG_INFO("Got response with size: " << nmemb);
    return nmemb;
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
    auto num_to_read = nmemb;
    if (m_read_offset + nmemb > m_working_request.body().size()) {
        num_to_read = m_working_request.body().size();
    }
    LOG_INFO("Returning " << num_to_read << " bytes ");
    for (std::size_t idx = 0; idx < num_to_read; idx++) {
        buffer[idx] = m_working_request.body()[m_read_offset + idx];
    }
    m_read_offset += num_to_read;
    LOG_INFO("New offset is " << m_read_offset << " bytes ");
    return num_to_read;
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

    m_handle = curl_easy_init();
    if (m_handle == nullptr) {
        throw std::runtime_error("Failed to initialize curl session");
    }

    m_error_buffer = std::string(CURL_ERROR_SIZE, ' ');

    auto rc =
        curl_easy_setopt(m_handle, CURLOPT_ERRORBUFFER, m_error_buffer.data());
    if (rc != CURLE_OK) {
        throw std::runtime_error("Failed to set curl error buffer");
    }

    rc = curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, curl_write_data);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl writefunction with error: " + m_error_buffer);
    }

    rc = curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl writedata with error: " + m_error_buffer);
    }

    rc = curl_easy_setopt(m_handle, CURLOPT_READFUNCTION, curl_read_data);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl readfunction with error: " + m_error_buffer);
    }

    rc = curl_easy_setopt(m_handle, CURLOPT_READDATA, this);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl readdata with error: " + m_error_buffer);
    }

    rc = curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1L);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl writedata with error: " + m_error_buffer);
    }
}

void CurlClient::prepare_put(const HttpRequest& request)
{
    auto rc = curl_easy_setopt(m_handle, CURLOPT_UPLOAD, 1L);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl to upload mode with error: " + m_error_buffer);
    }

    if (!request.body().empty()) {
        rc = curl_easy_setopt(
            m_handle, CURLOPT_INFILESIZE, request.body().size());
        if (rc != CURLE_OK) {
            throw std::runtime_error(
                "Failed to set curl request body size with error: "
                + m_error_buffer);
        }
    }
}

void CurlClient::prepare_get(const HttpRequest&)
{
    auto rc = curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1L);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl to get mode with error: " + m_error_buffer);
    }
}

HttpResponse::Ptr CurlClient::make_request(const HttpRequest& request)
{
    if (m_handle == nullptr) {
        initialize();
    }

    if (request.get_method() == HttpRequest::Method::GET) {
        prepare_get(request);
    }
    else if (request.get_method() == HttpRequest::Method::PUT) {
        prepare_put(request);
    }

    m_working_response = HttpResponse::create();
    m_working_request  = request;

    const auto url = request.get_path();
    curl_easy_setopt(m_handle, CURLOPT_URL, url.c_str());

    LOG_INFO("Making request to: " << url);
    LOG_INFO(request.to_string());
    auto rc = curl_easy_perform(m_handle);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed request with error: " + m_error_buffer);
    }

    LOG_INFO("Request all done");

    return std::move(m_working_response);
}

}  // namespace hestia