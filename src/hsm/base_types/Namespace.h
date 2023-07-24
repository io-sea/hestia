#pragma once

#include "HsmObject.h"
#include "Model.h"

#include <string>
#include <unordered_map>

namespace hestia {
class Namespace : public Model {
  public:
    Namespace();

    Namespace(const std::string& name);

    static std::string get_type();

  private:
    void init();
    StringField m_name{"name"};
    StringField m_data{"data"};
};
}  // namespace hestia