#pragma once

#include "HsmObject.h"
#include "StringAdapter.h"

#include <memory>
#include <sstream>

namespace hestia {

class HsmObjectJsonAdapter : public StringAdapter<HsmObject> {
  public:
    HsmObjectJsonAdapter(const std::string& internal_key_prefix);

    void to_string(const HsmObject& item, std::string& output) const override;

    void from_string(const std::string& output, HsmObject& item) const override;

  private:
    std::string m_internal_prefix{"_internal_"};
};

}  // namespace hestia