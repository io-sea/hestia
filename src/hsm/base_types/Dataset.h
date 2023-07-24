#pragma once

#include "HsmObject.h"
#include "LockableOwnableModel.h"
#include "Namespace.h"

namespace hestia {
class Dataset : public HsmItem, public LockableOwnableModel {
  public:
    Dataset();

    Dataset(const std::string& id);

    Dataset(const Dataset& other);

    static std::string get_type();

    const std::vector<HsmObject>& objects() const;

    const std::vector<Namespace>& namespaces() const;

    Dataset& operator=(const Dataset& other);

  private:
    void init();

    ForeignKeyProxyField<HsmObject> m_objects{"objects"};
    ForeignKeyProxyField<Namespace> m_namespaces{"namespaces"};
};
}  // namespace hestia