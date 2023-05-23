#include "RbhEvent.h"

#include "Dictionary.h"
#include "HashUtils.h"
#include "Logger.h"
#include "Metadata.h"
#include "YamlUtils.h"

#include <string>

namespace hestia {

std::string RbhEvent::type_to_string(const RbhTypes& event_type)
{
    switch (event_type) {
        case RbhTypes::UPSERT:
            return "upsert";
        case RbhTypes::LINK:
            return "link";
        case RbhTypes::DELETE:
            return "delete";
        case RbhTypes::UNLINK:
            return "unlink";
        case RbhTypes::NS_XATTR:
            return "ns_xattr";
        case RbhTypes::INODE_XATTR:
            return "inode_xattr";
    }
}

RbhEvent::RbhEvent(const RbhTypes& event_type, const Metadata& meta)
{
    m_type = event_type;
    m_meta = meta;
}


void RbhEvent::to_string(std::string& out, const bool sorted) const
{
    Dictionary dict = Dictionary(Dictionary::Type::MAP);

    dict.set_map_item("root", Dictionary::create());

    auto root = dict.get_map_item("root");

    switch (m_type) {
        case RbhTypes::UPSERT:
            upsert(*root);
            break;
        case RbhTypes::LINK:
            link(*root);
            break;
        case RbhTypes::DELETE:
            del(*root);
            break;
        case RbhTypes::UNLINK:
            unlink(*root);
            break;
        case RbhTypes::NS_XATTR:
            ns_xattr(*root);
            break;
        case RbhTypes::INODE_XATTR:
            inode_xattr(*root);
    }

    return YamlUtils::dict_to_yaml(dict, out, sorted);
}

void add_scalar(
    Dictionary& dict,
    const std::string& key,
    const std::string& value,
    const std::string& tag         = "",
    const std::string& prefix      = "",
    const std::string& default_val = "")
{
    if (dict.get_type() != Dictionary::Type::MAP) {
        LOG_ERROR("Dictionary must be map to set scalar by key");
        return;
    }

    if (value.empty() && default_val.empty()) {
        return;
    }

    dict.set_map_item(key, Dictionary::create(Dictionary::Type::SCALAR));
    if (value.empty()) {
        dict.get_map_item(key)->set_scalar(default_val);
    }
    else {
        dict.get_map_item(key)->set_scalar(value);
    }
    dict.get_map_item(key)->set_tag(tag, prefix);
}

void set_id(
    Dictionary& dict, const std::string& id, const std::string& key = "id")
{
    auto encoded_id = HashUtils::base64_encode(id);
    add_scalar(dict, key, encoded_id, "binary", "!!");
}

void set_xattrs(Dictionary& dict, const Metadata& meta)
{
    dict.set_map_item("xattrs", Dictionary::create(Dictionary::Type::MAP));
    auto xattrs = dict.get_map_item("xattrs");

    auto tier = meta.get_item("tier");
    if (tier.empty()) {
        add_scalar(
            *xattrs, "source_tier", meta.get_item("source_tier"), "uint32");
        add_scalar(
            *xattrs, "target_tier", meta.get_item("target_tier"), "uint32");
    }
    else {
        add_scalar(*xattrs, "tier", meta.get_item("tier"), "uint32");
    }

    // TODO: Add other HSM specific metadata
}

void set_xtime(
    Dictionary& dict, const Metadata& meta, const std::string& prefix)
{
    auto sec  = meta.get_item(prefix + "sec");
    auto nsec = meta.get_item(prefix + "nsec");

    if (sec.empty() && nsec.empty()) {
        return;
    }

    dict.set_map_item(
        prefix + "time", Dictionary::create(Dictionary::Type::MAP));
    auto xtime = dict.get_map_item(prefix + "time");

    add_scalar(*xtime, "sec", sec, "int64");
    add_scalar(*xtime, "nsec", nsec, "uint32");
}

void set_xdev(
    Dictionary& dict, const Metadata& meta, const std::string prefix = "")
{
    auto major = meta.get_item(prefix + "major");
    auto minor = meta.get_item(prefix + "minor");

    if (major.empty() && minor.empty()) {
        return;
    }

    dict.set_map_item(
        prefix + "dev", Dictionary::create(Dictionary::Type::MAP));
    auto xdev = dict.get_map_item(prefix + "dev");

    add_scalar(*xdev, "major", major, "uint32");
    add_scalar(*xdev, "minor", minor, "uint32");
}

void RbhEvent::upsert(Dictionary& dict) const
{
    dict.set_tag(type_to_string(RbhTypes::UPSERT));

    // ID Field
    set_id(dict, m_meta.get_item("id"));

    // Symlink
    add_scalar(dict, "symlink", "n", "bool", "!!");

    // Xattrs
    set_xattrs(dict, m_meta);

    // Statx
    dict.set_map_item("statx", Dictionary::create(Dictionary::Type::MAP));
    auto statx = dict.get_map_item("statx");

    add_scalar(*statx, "type", "file");  // TODO: Do we support other types?
    add_scalar(*statx, "mode", "0777");  // TODO: Do we support permissions?

    // TODO: Do we have this data?
    statx->set_map_item(
        "attributes", Dictionary::create(Dictionary::Type::MAP));
    auto attr = statx->get_map_item("attributes");

    add_scalar(
        *attr, "compressed", m_meta.get_item("compressed"), "bool", "!!", "n");
    add_scalar(
        *attr, "immutable", m_meta.get_item("immutable"), "bool", "!!", "n");
    add_scalar(*attr, "append", m_meta.get_item("append"), "bool", "!!", "n");
    add_scalar(*attr, "nodump", m_meta.get_item("nodump"), "bool", "!!", "n");
    add_scalar(
        *attr, "encrypted", m_meta.get_item("encrypted"), "bool", "!!", "n");
    add_scalar(
        *attr, "automount", m_meta.get_item("automount"), "bool", "!!", "n");
    add_scalar(
        *attr, "mount-root", m_meta.get_item("mount-root"), "bool", "!!", "n");
    add_scalar(*attr, "verity", m_meta.get_item("verity"), "bool", "!!", "n");
    add_scalar(*attr, "dax", m_meta.get_item("dax"), "bool", "!!", "n");


    add_scalar(*statx, "nlink", m_meta.get_item("nlink"), "uint32");
    add_scalar(*statx, "uid", m_meta.get_item("uid"), "uint32");
    add_scalar(*statx, "gid", m_meta.get_item("gid"), "uint32");
    add_scalar(*statx, "ino", m_meta.get_item("ino"), "uint64");
    add_scalar(*statx, "size", m_meta.get_item("size"), "uint64");
    add_scalar(*statx, "blocks", m_meta.get_item("blocks"), "uint64");
    add_scalar(*statx, "blksize", m_meta.get_item("blksize"), "uint32");

    set_xtime(*statx, m_meta, "a");
    set_xtime(*statx, m_meta, "b");
    set_xtime(*statx, m_meta, "c");
    set_xtime(*statx, m_meta, "m");

    set_xdev(*statx, m_meta);
    set_xdev(*statx, m_meta, "r");
}

void RbhEvent::del(Dictionary& dict) const
{
    dict.set_tag(type_to_string(RbhTypes::DELETE));

    set_id(dict, m_meta.get_item("id"));
}

void RbhEvent::link(Dictionary& dict) const
{
    dict.set_tag(type_to_string(RbhTypes::LINK));

    set_id(dict, m_meta.get_item("id"));

    set_xattrs(dict, m_meta);

    set_id(dict, m_meta.get_item("parent"), "parent");

    add_scalar(dict, "name", m_meta.get_item("name"));
}

void RbhEvent::unlink(Dictionary& dict) const
{
    dict.set_tag(type_to_string(RbhTypes::UNLINK));

    set_id(dict, m_meta.get_item("id"));

    set_id(dict, m_meta.get_item("parent"), "parent");

    add_scalar(dict, "name", m_meta.get_item("name"));
}

void RbhEvent::ns_xattr(Dictionary& dict) const
{
    dict.set_tag(type_to_string(RbhTypes::NS_XATTR));

    set_id(dict, m_meta.get_item("id"));

    set_xattrs(dict, m_meta);

    set_id(dict, m_meta.get_item("parent"), "parent");

    add_scalar(dict, "name", m_meta.get_item("name"));
}

void RbhEvent::inode_xattr(Dictionary& dict) const
{
    dict.set_tag(type_to_string(RbhTypes::INODE_XATTR));

    set_id(dict, m_meta.get_item("id"));

    set_xattrs(dict, m_meta);
}

}  // namespace hestia