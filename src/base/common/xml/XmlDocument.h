#pragma once
#include <memory>
#include <string>

#include "XmlElement.h"
#include "XmlProlog.h"

namespace hestia {
class XmlElement;
class XmlProlog;
using XmlPrologPtr  = std::unique_ptr<XmlProlog>;
using XmlElementPtr = std::unique_ptr<XmlElement>;

class XmlDocument {
  public:
    XmlDocument();
    virtual ~XmlDocument();

    static std::unique_ptr<XmlDocument> create();

    using Ptr = std::unique_ptr<XmlDocument>;

    bool has_root() const;

    XmlProlog* get_prolog() const;
    XmlElement* get_root() const;

    void set_prolog(XmlPrologPtr prolog);
    void set_root(XmlElementPtr root);

    std::string to_string() const;

  private:
    XmlPrologPtr m_prolog;
    XmlElementPtr m_root;
};
}  // namespace hestia