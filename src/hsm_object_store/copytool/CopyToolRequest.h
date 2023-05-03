#pragma once

#include "HsmObjectStoreRequest.h"

#include <sstream>
#include <string>

namespace hestia {
class CopyToolRequest {
  public:
    CopyToolRequest(const HsmObjectStoreRequest& request);

    CopyToolRequest(const std::string& request_str);

    void deserialize(const std::string& request_str);

    const HsmObjectStoreRequest& get_object_store_request() const;

    std::string serialize() const;

  private:
    static constexpr char object_token[] = "ObjectId: ";
    static constexpr char offset_token[] = "Offset: ";
    static constexpr char length_token[] = "Length: ";
    static constexpr char source_token[] = "SourceTier: ";
    static constexpr char target_token[] = "TargetTier: ";

    HsmObjectStoreRequest m_object_store_request;
};
}  // namespace hestia