#include "StaticContentView.h"

#include "File.h"
#include "Logger.h"

namespace hestia {
StaticContentView::StaticContentView(const std::string& directory, bool cache) :
    WebView(), m_directory(directory), m_should_cache(cache)
{
    if (!directory.empty()) {
        LOG_INFO("Serving static content from: " << directory);
    }
}

void StaticContentView::set_buffer(const std::string& buffer)
{
    m_buffer = buffer;
    m_type   = Type::BUFFER;
}

HttpResponse::Ptr StaticContentView::on_get(
    const HttpRequest& request, HttpEvent event, const AuthorizationContext&)
{
    if (event != HttpEvent::EOM) {
        return HttpResponse::create(
            HttpResponse::CompletionStatus::AWAITING_EOM);
    }

    if (m_type == Type::BUFFER) {
        auto response = HttpResponse::create();
        response->set_body(m_buffer);
        response->header().set_content_type("text/html");
        return response;
    }


    auto path = request.get_path();
    if (path.empty() || path == "/") {
        path = "index.html";
    }

    auto nav_up_index = path.find("..");
    if (nav_up_index != std::string::npos) {
        LOG_ERROR("Failed to due attempt to move up directory.");
        return HttpResponse::create(400, "Bad Request");
    }

    auto ext_index = path.rfind(".");
    if (ext_index == 0) {
        path += ".html";
    }
    else {
        /*
        const auto extension = path.substr(ext_index, path.size() - ext_index);
        if (extension != ".js" || extension != ".css")
        {
            LOG_INFO("Requested unsupported extension: " << path);
            return HttpResponse::create(404, "Not Found");
        }
        */
    }

    if (m_should_cache) {
        if (auto iter = m_cache.find(path); iter != m_cache.end()) {
            auto response = HttpResponse::create();
            response->set_body(iter->second);
            return response;
        }

        std::string body;
        auto found = read(path, body);
        if (!found) {
            LOG_INFO("Requested asset not found: " << path);
            return HttpResponse::create(404, "Not Found");
        }

        // TODO - should be under a lock for multi-threading
        m_cache[path] = body;
        auto response = HttpResponse::create();
        response->set_body(body);
        response->header().set_content_type("text/html");
        return response;
    }
    else {
        std::string body;
        auto found = read(path, body);
        if (!found) {
            LOG_INFO("Requested asset not found: " << path);
            return HttpResponse::create(404, "Not Found");
        }

        auto response = HttpResponse::create();
        response->set_body(body);
        response->header().set_content_type("text/html");
        return response;
    }
}

bool StaticContentView::read(
    const std::string& path, std::string& content) const
{
    auto resource_path = m_directory / std::filesystem::path(path);
    if (std::filesystem::is_regular_file(resource_path)) {
        File file(resource_path);
        file.read(content);
        return true;
    }
    return false;
}
}  // namespace hestia