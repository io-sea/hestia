#include "CurlHandle.h"

#include "Logger.h"

namespace hestia {
CurlHandle::CurlHandle()
{
    setup();
}

CurlHandle::~CurlHandle()
{
    free();
}

void CurlHandle::setup()
{
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

    rc = curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1L);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl writedata with error: " + m_error_buffer);
    }
}

void CurlHandle::free()
{
    if (m_handle != nullptr) {
        curl_easy_cleanup(m_handle);
        m_handle = nullptr;
    }

    if (m_headers != nullptr) {
        curl_slist_free_all(m_headers);
        m_headers = nullptr;
    }
}

void CurlHandle::prepare_put(const HttpRequest& request, Stream* stream)
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
    else if (stream != nullptr && stream->has_content()) {
        rc = curl_easy_setopt(
            m_handle, CURLOPT_INFILESIZE, stream->get_source_size());
        if (rc != CURLE_OK) {
            throw std::runtime_error(
                "Failed to set curl request body size with error: "
                + m_error_buffer);
        }
    }
}

void CurlHandle::prepare_get()
{
    auto rc = curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1L);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl to get mode with error: " + m_error_buffer);
    }
}

void CurlHandle::prepare_post()
{
    auto rc = curl_easy_setopt(m_handle, CURLOPT_POST, 1L);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl to get mode with error: " + m_error_buffer);
    }
    rc = curl_easy_setopt(m_handle, CURLOPT_POSTFIELDSIZE, 0L);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl to get mode with error: " + m_error_buffer);
    }
}

void CurlHandle::prepare_head()
{
    auto rc = curl_easy_setopt(m_handle, CURLOPT_NOBODY, 1L);
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl to get mode with error: " + m_error_buffer);
    }
}

void CurlHandle::prepare_delete()
{
    auto rc = curl_easy_setopt(m_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    if (rc != CURLE_OK) {
        throw std::runtime_error(
            "Failed to set curl to delete mode with error: " + m_error_buffer);
    }
}

void CurlHandle::prepare_headers(const HttpHeader& headers)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        if (value.empty()) {
            m_headers =
                curl_slist_append(m_headers, std::string(key + ":").c_str());
        }
        else {
            m_headers = curl_slist_append(
                m_headers, std::string(key + ": " + value).c_str());
        }
    };
    headers.for_each(on_item);

    if (m_headers != nullptr) {
        curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, m_headers);
    }
}

}  // namespace hestia