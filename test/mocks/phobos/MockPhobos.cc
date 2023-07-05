#include "MockPhobos.h"

#include <stdexcept>
#include <unistd.h>

#include <iostream>

int MockPhobos::phobos_put(
    pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata)
{
    (void)n;
    (void)cb;
    (void)udata;

    auto fd   = xfers[0].xd_fd;
    auto size = xfers[0].xd_put_params.size;
    auto id   = xfers[0].xd_objid;

    m_metadata_cache[id] = xfers[0].xd_attrs.attr_set;

    std::vector<char> buffer(size);
    if (size > 0) {
        auto rc = ::read(fd, &buffer[0], size);
        if (rc < 0) {
            return rc;
        }

        if (rc != size) {
            return -1;
        }
    }
    m_data_cache[id] = buffer;
    return 0;
}

int MockPhobos::phobos_get(
    pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata)
{
    (void)n;
    (void)cb;
    (void)udata;

    auto id   = xfers[0].xd_objid;
    auto iter = m_data_cache.find(id);
    if (iter == m_data_cache.end()) {
        return -1;
    }

    auto fd     = xfers[0].xd_fd;
    auto buffer = &(iter->second)[0];
    auto size   = iter->second.size();

    ssize_t rc = ::write(fd, buffer, size);
    if (static_cast<std::size_t>(rc) < size) {
        return -2;
    }
    return 0;
}

int MockPhobos::phobos_getmd(
    pho_xfer_desc* xfers, size_t n, pho_completion_cb_t cb, void* udata)
{
    (void)n;
    (void)cb;
    (void)udata;

    auto id   = xfers[0].xd_objid;
    auto iter = m_metadata_cache.find(id);
    if (iter == m_metadata_cache.end()) {
        return -1;
    }

    xfers[0].xd_attrs.attr_set = iter->second;
    return 0;
}

int MockPhobos::phobos_delete(pho_xfer_desc* xfers, size_t num_xfers)
{
    (void)num_xfers;

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

void MockPhobos::phobos_store_object_list_free(object_info* objs, int n_objs)
{
    (void)objs;
    (void)n_objs;

    delete objs;
}

int MockPhobos::phobos_store_object_list(
    const char** res,
    int n_res,
    bool is_pattern,
    const char** metadata,
    int n_metadata,
    bool deprecated,
    object_info** objs,
    int* n_objs)
{
    (void)res;
    (void)n_res;
    (void)is_pattern;
    (void)deprecated;

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