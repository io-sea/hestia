#pragma once

#include "Dictionary.h"
#include "Map.h"

#include <string>

namespace hestia {

class RbhEvent {
  public:
    enum class RbhTypes {  // Events Robinhood recognizes
        UPSERT,
        DELETE,
        LINK,
        UNLINK,
        NS_XATTR,
        INODE_XATTR
    };

    static std::string type_to_string(const RbhTypes& event_type);

    RbhEvent(const RbhTypes& event_type, const Map& meta);

    void to_string(std::string& out, const bool sorted = false) const;

  private:
    void upsert(Dictionary& dict) const;
    void del(Dictionary& dict) const;
    void link(Dictionary& dict) const;
    void unlink(Dictionary& dict) const;
    void ns_xattr(Dictionary& dict) const;
    void inode_xattr(Dictionary& dict) const;


    RbhTypes m_type;
    Map m_meta;
};
}  // namespace hestia