#pragma once

#include "Dictionary.h"
#include "EventSink.h"
#include "Map.h"

#include <string>

namespace hestia {

class HsmService;

// mayb this will be a base class and have a buffer (for test/debug) and
// file based derived versions.
class HsmEventSink : public EventSink {
  public:
    HsmEventSink(HsmService* hsm_service);

    void on_event(const CrudEvent& event) override;

    /*
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
    */

  private:
    HsmService* m_hsm_service{nullptr};
    /*
    void upsert(Dictionary& dict) const;
    void del(Dictionary& dict) const;
    void link(Dictionary& dict) const;
    void unlink(Dictionary& dict) const;
    void ns_xattr(Dictionary& dict) const;
    void inode_xattr(Dictionary& dict) const;
    */

    /*
    RbhTypes m_type;
    Map m_meta;
    */
};
}  // namespace hestia