#include "PhobosInterfaceImpl.h"

#if HAS_PHOBOS
#include "PhobosDescriptor.h"

#include <sstream>
#include <stdexcept>

#include <unistd.h>

namespace hestia {
void PhobosInterfaceImpl::init()
{
    phobos_init_cpp();
}

void PhobosInterfaceImpl::finish()
{
    phobos_fini_cpp();
}

void PhobosInterfaceImpl::get(const StorageObject& obj, int fd)
{
    PhobosDescriptor desc({obj.id(), PhobosDescriptor::Operation::GET, fd});
    ssize_t rc = phobos_get_cpp(&desc.get_handle(), 1, nullptr, nullptr);
    if (rc != 0) {
        throw std::runtime_error("phobos_get " + std::to_string(rc));
    }
}

void PhobosInterfaceImpl::put(const StorageObject& obj, int fd)
{
    PhobosDescriptor desc({obj.id(), PhobosDescriptor::Operation::PUT});
    if (fd > -1) {
        desc.set_fd(fd);
        desc.set_size(obj.size());
    }

    auto each_item = [&desc](const std::string& key, const std::string& value) {
        int rc = pho_attr_set(
            &desc.get_handle().xd_attrs, key.c_str(), value.c_str());
        if (rc != 0) {
            throw std::runtime_error("Phobos_file::set_meta_data");
        }
    };
    obj.metadata().for_each_item(each_item);

    pho_completion_cb_t callback = [](void*, const struct pho_xfer_desc*,
                                      int rc) {
        if (rc != 0) {
            throw std::runtime_error("phobos_put " + std::to_string(rc));
        }
    };

    ssize_t rc = phobos_put_cpp(&desc.get_handle(), 1, callback, nullptr);
    if (rc != 0) {
        throw std::runtime_error("phobos_put " + std::to_string(rc));
    }
}

void PhobosInterfaceImpl::get_metadata(StorageObject& obj)
{
    PhobosDescriptor descriptor(
        {obj.id(), PhobosDescriptor::Operation::GET_MD});

    int rc = phobos_getmd_cpp(&descriptor.get_handle(), 1, nullptr, nullptr);
    if (rc != 0) {
        throw std::runtime_error("Phobos_file::get_meta_data");
    }

    auto on_metadata_item = [](const char* key, const char* value,
                               void* udata) {
        auto obj = reinterpret_cast<StorageObject*>(udata);
        obj->get_metadata_as_writeable().set_item(key, value);
        return 0;
    };
    rc = phobos_attrs_foreach_cpp(
        &descriptor.get_handle().xd_attrs, on_metadata_item, &obj);
    if (rc != 0) {
        throw std::runtime_error("Phobos_file::get_meta_data");
    }
}

bool PhobosInterfaceImpl::exists(const StorageObject& obj)
{
    PhobosDescriptor descriptor(
        {obj.id(), PhobosDescriptor::Operation::GET_MD});
    bool exists =
        phobos_getmd_cpp(&descriptor.get_handle(), 1, nullptr, nullptr) == 0;
    return exists;
}

void PhobosInterfaceImpl::remove(const StorageObject& obj)
{
    PhobosDescriptor descriptor({obj.id(), PhobosDescriptor::Operation::DEL});

    phobos_delete_cpp(&descriptor.get_handle(), 1);
}

void from_string(Map& metadata, const std::string& str)
{
    std::stringstream ss(str);
    std::string key, value, dump;
    while (ss.good()) {
        getline(ss, dump, '"');
        getline(ss, key, '"');
        getline(ss, dump, '"');
        getline(ss, value, '"');
        metadata.set_item(key, value);
    }
}

void PhobosInterfaceImpl::list(
    const KeyValuePair& query, std::vector<StorageObject>& found)
{
    const std::string skey = query.first + "=" + query.second;
    const char* key        = skey.c_str();
    int query_count        = (!query.first.empty() ? 1 : 0);

    int num_objects{0};
    struct object_info* obj_info{nullptr};

    const std::string spattern;
    const char* pattern = spattern.c_str();
    int rc              = phobos_store_object_list_cpp(
        &pattern, 1, true, &key, query_count, false, &obj_info, &num_objects);
    if (rc != 0) {
        throw std::runtime_error("Phobos_file::get_object_list");
    }

    for (int idx = 0; idx < num_objects; idx++) {
        StorageObject obj(obj_info[idx].oid);
        from_string(obj.get_metadata_as_writeable(), obj_info[idx].user_md);
        found.push_back(obj);
    }

    phobos_store_object_list_free_cpp(obj_info, num_objects);
}
}  // namespace hestia

#endif