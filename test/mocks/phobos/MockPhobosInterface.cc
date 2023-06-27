#include "MockPhobosInterface.h"

#include "UuidUtils.h"

#include <sstream>
#include <unistd.h>

namespace hestia::mock {

MockPhobosInterface::MockPhobosInterface() {}

std::unique_ptr<MockPhobosInterface> MockPhobosInterface::create()
{
    return std::make_unique<MockPhobosInterface>();
}

void MockPhobosInterface::get(const StorageObject& obj, int fd)
{
    const auto id_str = UuidUtils::to_string(obj.id());

    pho_xfer_desc desc;
    desc.xd_objid = id_str;
    desc.xd_op    = PHO_XFER_OP_GET;
    desc.xd_fd    = fd;

    auto rc = m_phobos.phobos_get(&desc, 1, nullptr, nullptr);
    if (fd > 0) {
        ::close(fd);
    }

    if (rc != 0) {
        throw std::runtime_error("phobos_get " + std::to_string(rc));
    }
}

void MockPhobosInterface::put(const StorageObject& obj, int fd)
{
    const auto id_str = UuidUtils::to_string(obj.id());

    pho_xfer_desc desc;
    desc.xd_objid = id_str;
    desc.xd_op    = PHO_XFER_OP_PUT;

    if (fd > -1) {
        desc.xd_fd              = fd;
        desc.xd_put_params.size = obj.m_size;
    }

    auto each_item = [&desc](const std::string& key, const std::string& value) {
        desc.xd_attrs.attr_set.set_item(key, value);
    };
    obj.m_metadata.for_each_item(each_item);

    ssize_t rc = m_phobos.phobos_put(&desc, 1, nullptr, nullptr);
    if (fd > 0) {
        ::close(fd);
    }

    if (rc != 0) {
        throw std::runtime_error("phobos_put " + std::to_string(rc));
    }
}

bool MockPhobosInterface::exists(const StorageObject& obj)
{
    const auto id_str = UuidUtils::to_string(obj.id());

    pho_xfer_desc desc;
    desc.xd_objid = id_str;
    desc.xd_op    = PHO_XFER_OP_GETMD;

    bool exists = m_phobos.phobos_getmd(&desc, 1, nullptr, nullptr) == 0;
    return exists;
}

void MockPhobosInterface::get_metadata(StorageObject& obj)
{
    const auto id_str = UuidUtils::to_string(obj.id());

    pho_xfer_desc desc;
    desc.xd_objid = id_str;
    desc.xd_op    = PHO_XFER_OP_GETMD;

    int rc = m_phobos.phobos_getmd(&desc, 1, nullptr, nullptr);
    if (rc != 0) {
        throw std::runtime_error("Phobos_file::get_meta_data");
    }

    auto on_metadata_item = [](const char* key, const char* value,
                               void* udata) {
        auto obj = reinterpret_cast<StorageObject*>(udata);
        obj->m_metadata.set_item(key, value);
        return 0;
    };
    rc = m_phobos.phobos_attrs_foreach(&desc.xd_attrs, on_metadata_item, &obj);
    if (rc != 0) {
        throw std::runtime_error("Phobos_file::get_meta_data");
    }
}

void MockPhobosInterface::remove(const StorageObject& obj)
{
    const auto id_str = UuidUtils::to_string(obj.id());

    pho_xfer_desc desc;
    desc.xd_objid = id_str;
    desc.xd_op    = PHO_XFER_OP_DEL;

    m_phobos.phobos_delete(&desc, 1);
}

void from_string(Metadata& metadata, const std::string& str)
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

void MockPhobosInterface::list(
    const Metadata::Query& query, std::vector<StorageObject>& found)
{
    const char* key = query.first.c_str();
    int query_count = (!query.first.empty() ? 1 : 0);

    int num_objects{0};
    struct object_info* obj_info{nullptr};

    const char* pattern = "";
    int rc              = m_phobos.phobos_store_object_list(
        &pattern, 1, true, &key, query_count, false, &obj_info, &num_objects);
    if (rc != 0) {
        throw std::runtime_error("Phobos_file::get_object_list");
    }

    for (int idx = 0; idx < num_objects; idx++) {
        StorageObject obj(obj_info[idx].oid);
        from_string(obj.m_metadata, obj_info[idx].user_md);
        obj.m_metadata.set_item("key", obj_info[idx].oid);
        found.push_back(obj);
    }

    m_phobos.phobos_store_object_list_free(obj_info, num_objects);
}
}  // namespace hestia::mock