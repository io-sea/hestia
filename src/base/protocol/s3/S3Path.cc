#include "S3Path.h"

namespace hestia {

S3Path::S3Path(const HttpRequest& request)
{
    if (const auto& host = request.get_header().get_item("Host");
        !host.empty()) {
        m_container_name     = host;
        const auto full_path = request.get_path();
        m_object_id          = full_path.substr(1, full_path.size());
    }
    else {
        from_path_only(request.get_path());
    }
}

S3Path::S3Path(const std::string& path)
{
    from_path_only(path);
}

void S3Path::from_path_only(const std::string& path)
{
    if (path.empty() || path == "/") {
        return;
    }

    const auto path_no_slash = path.substr(1, path.size());
    if (auto slash_index = path_no_slash.find('/');
        slash_index == path_no_slash.npos) {
        m_container_name = path_no_slash;
    }
    else {
        m_container_name = path_no_slash.substr(0, slash_index);
        m_object_id =
            path_no_slash.substr(slash_index + 1, path_no_slash.size());
    }
}

}  // namespace hestia