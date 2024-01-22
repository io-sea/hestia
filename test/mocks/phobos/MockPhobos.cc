#include "MockPhobos.h"

#include "File.h"

#include <filesystem>
#include <stdexcept>
#include <unistd.h>

#include <iostream>

void MockPhobos::write_md(const std::string& id, const hestia::Map& map)
{
    if (m_root.empty()) {
        m_metadata_cache[id] = map;
    }
    else {
        hestia::File f(m_root + "/" + id + ".meta");
        const auto content = map.to_string();
        f.write(content.c_str(), content.size());
    }
}

void MockPhobos::write_data(
    const std::string& id, const std::vector<char>& data)
{
    if (m_root.empty()) {
        m_data_cache[id] = data;
    }
    else {
        hestia::File f(m_root + "/" + id + ".data");
        f.write(data.data(), data.size());
    }
}

int MockPhobos::phobos_put(
    pho_xfer_desc* xfers, size_t, pho_completion_cb_t, void*)
{
    auto fd   = xfers[0].xd_fd;
    auto size = xfers[0].xd_put_params.size;
    auto id   = xfers[0].xd_objid;

    write_md(id, xfers[0].xd_attrs.attr_set);

    std::vector<char> buffer(size, 0);
    std::size_t offset = 0;
    int remainder      = size - offset;
    while (remainder > 0) {
        const auto rc =
            ::read(fd, &buffer[offset], static_cast<size_t>(remainder));
        if (rc < 0) {
            return rc;
        }
        offset += rc;
        remainder = size - offset;
    }
    write_data(id, buffer);
    return 0;
}

bool MockPhobos::read_md(const std::string& id, hestia::Map& map)
{
    if (m_root.empty()) {
        auto iter = m_metadata_cache.find(id);
        if (iter == m_metadata_cache.end()) {
            return false;
        }
        map = iter->second;
        return true;
    }
    else {
        const auto path = m_root + "/" + id + ".meta";
        if (std::filesystem::exists(path)) {
            hestia::File f(path);
            std::string content;
            f.read(content);
            map.from_string(content);
            return true;
        }
        return false;
    }
}

bool MockPhobos::read_data(const std::string& id, std::vector<char>& data)
{
    if (m_root.empty()) {
        auto iter = m_data_cache.find(id);
        if (iter == m_data_cache.end()) {
            return false;
        }
        data = iter->second;
        return true;
    }
    else {
        const auto path = m_root + "/" + id + ".data";
        if (std::filesystem::exists(path)) {
            hestia::File f(path);
            std::string content;
            f.read(content);
            data = std::vector<char>(content.begin(), content.end());
            return true;
        }
        return false;
    }
}

bool MockPhobos::remove_data(const std::string& id)
{
    if (m_root.empty()) {
        auto data_iter = m_data_cache.find(id);
        if (data_iter != m_data_cache.end()) {
            m_data_cache.erase(data_iter);
        }
    }
    else {
        const auto path = m_root + "/" + id + ".data";
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
    }
    return true;
}

bool MockPhobos::remove_md(const std::string& id)
{
    if (m_root.empty()) {
        auto meta_iter = m_metadata_cache.find(id);
        if (meta_iter == m_metadata_cache.end()) {
            return false;
        }
        m_metadata_cache.erase(meta_iter);
        return true;
    }
    else {
        const auto path = m_root + "/" + id + ".meta";
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
        return true;
    }
}

int MockPhobos::phobos_get(
    pho_xfer_desc* xfers, size_t, pho_completion_cb_t, void*)
{
    auto id = xfers[0].xd_objid;
    std::vector<char> data;
    if (!read_data(id, data)) {
        return -1;
    }

    auto fd     = xfers[0].xd_fd;
    auto buffer = &data[0];
    auto size   = data.size();
    ssize_t rc  = ::write(fd, buffer, size);
    if (static_cast<std::size_t>(rc) < size) {
        return -2;
    }
    return 0;
}

int MockPhobos::phobos_getmd(
    pho_xfer_desc* xfers, size_t, pho_completion_cb_t, void*)
{
    auto id = xfers[0].xd_objid;
    if (!read_md(id, xfers[0].xd_attrs.attr_set)) {
        return -1;
    }
    return 0;
}

int MockPhobos::phobos_delete(pho_xfer_desc* xfers, size_t)
{
    auto id = xfers[0].xd_objid;

    auto meta_iter = m_metadata_cache.find(id);
    if (meta_iter == m_metadata_cache.end()) {
        return -1;
    }
    m_metadata_cache.erase(meta_iter);

    auto data_iter = m_data_cache.find(id);
    if (data_iter != m_data_cache.end()) {
        m_data_cache.erase(data_iter);
    }
    return 0;
}

int MockPhobos::phobos_attrs_foreach(
    const pho_attrs* md, pho_attrs_iter_t cb, void* udata)
{
    auto each_item = [udata,
                      cb](const std::string& key, const std::string& value) {
        cb(key.c_str(), value.c_str(), udata);
    };
    md->attr_set.for_each_item(each_item);
    return 0;
}

void MockPhobos::pho_xfer_desc_destroy(pho_xfer_desc*) {}

void MockPhobos::phobos_store_object_list_free(object_info* objs, int)
{
    delete objs;
}

void MockPhobos::set_root(const std::string& root)
{
    m_root = root;
    if (std::filesystem::exists(m_root)) {
        std::filesystem::create_directories(m_root);
    }
}

int MockPhobos::phobos_store_object_list(
    const char**,
    int,
    bool,
    const char** metadata,
    int n_metadata,
    bool,
    object_info** objs,
    int* n_objs)
{
    *n_objs = 0;

    for (const auto& entry : m_metadata_cache) {
        for (int idx = 0; idx < n_metadata; idx++) {
            auto md_item = std::string(*(metadata + idx));
            if (auto first_eq = md_item.find_first_of("=");
                first_eq != md_item.length()) {
                md_item = md_item.substr(0, first_eq);
            }
            else {
                throw std::runtime_error("MockPhobos: Need value of query");
            }
            if (auto val = entry.second.get_item(md_item); !val.empty()) {
                auto obj_info     = new object_info;
                obj_info->oid     = entry.first;
                obj_info->user_md = "";
                *(objs + *n_objs) = obj_info;
                (*n_objs)++;
                break;
            }
        }
    }
    return 0;
}