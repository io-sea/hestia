#include "S3Path.h"

#include "StringUtils.h"
#include <iostream>

namespace hestia {

S3Path::S3Path(const HttpRequest& request, const std::string& domain)
{
    const auto host = request.get_header().get_item("Host");
    const auto path_less_domain =
        StringUtils::remove_prefix(request.get_path(), domain);
    if (domain.empty() || host.empty() || host == domain) {
        from_path_only(path_less_domain);
    }
    else {
        auto [bucket_name, rest] =
            StringUtils::split_on_first(host, "." + domain);
        m_bucket_name = bucket_name;
        object_from_path_only(path_less_domain);
    }
    /*
    std::cout << "domain is: " << domain << std::endl;
    std::cout << "path is: " << request.get_path() << std::endl;
    std::cout << "host is: " << host << std::endl;
    std::cout << "bucket is: " << m_bucket_name << std::endl;
    std::cout << "object is: " << m_object_key << std::endl;
    */
}

S3Path::S3Path(const std::string& path)
{
    from_path_only(path);
}

enum class LineState {
    AWAITING_FIRST_NON_SLASH,
    AWAITING_CONTAINER_END,
    AWAITING_OBJECT_END,
    AWAITING_QUERY_END
};

std::string S3Path::get_resource_path() const
{
    std::string path = "/" + m_bucket_name;
    if (!m_object_key.empty()) {
        path += "/" + m_object_key;
    }
    return path;
}

void S3Path::object_from_path_only(const std::string& path)
{
    if (path.empty() || path == "/") {
        return;
    }

    auto line_state = LineState::AWAITING_FIRST_NON_SLASH;
    for (const auto c : path) {
        if (c == '/') {
            if (line_state == LineState::AWAITING_FIRST_NON_SLASH) {
                continue;
            }
        }
        else {
            if (line_state == LineState::AWAITING_FIRST_NON_SLASH) {
                line_state = LineState::AWAITING_OBJECT_END;
                m_object_key += c;
            }
            else if (line_state == LineState::AWAITING_OBJECT_END) {
                if (m_object_key.empty() && c == '?') {
                    line_state = LineState::AWAITING_QUERY_END;
                }
                else {
                    m_object_key += c;
                }
            }
            else if (line_state == LineState::AWAITING_QUERY_END) {
                m_queries += c;
            }
        }
    }
}

void S3Path::from_path_only(const std::string& path)
{
    if (path.empty() || path == "/") {
        return;
    }

    auto line_state = LineState::AWAITING_FIRST_NON_SLASH;
    for (const auto c : path) {
        if (c == '/') {
            if (line_state == LineState::AWAITING_FIRST_NON_SLASH) {
                continue;
            }
            else if (line_state == LineState::AWAITING_CONTAINER_END) {
                line_state = LineState::AWAITING_OBJECT_END;
            }
        }
        else {
            if (line_state == LineState::AWAITING_FIRST_NON_SLASH) {
                line_state = LineState::AWAITING_CONTAINER_END;
                m_bucket_name += c;
            }
            else if (line_state == LineState::AWAITING_CONTAINER_END) {
                if (c == '?') {
                    line_state = LineState::AWAITING_QUERY_END;
                }
                else {
                    m_bucket_name += c;
                }
            }
            else if (line_state == LineState::AWAITING_OBJECT_END) {
                if (m_object_key.empty() && c == '?') {
                    line_state = LineState::AWAITING_QUERY_END;
                }
                else {
                    m_object_key += c;
                }
            }
            else if (line_state == LineState::AWAITING_QUERY_END) {
                m_queries += c;
            }
        }
    }
}

}  // namespace hestia