#pragma once

#include "HsmObjectStoreRequest.h"

#include <sstream>
#include <string>

class CopyToolRequest {
  public:
    CopyToolRequest(const HsmObjectStoreRequest& request);

    CopyToolRequest(const std::string& requestStr);

    void deserialize(const std::string& requestStr);

    const HsmObjectStoreRequest& getObjectStoreRequest() const;

    std::string serialize() const;

  private:
    static constexpr char OBJECT_TOKEN[] = "ObjectId: ";
    static constexpr char OFFSET_TOKEN[] = "Offset: ";
    static constexpr char LENGTH_TOKEN[] = "Length: ";
    static constexpr char SOURCE_TOKEN[] = "SourceTier: ";
    static constexpr char TARGET_TOKEN[] = "TargetTier: ";

    HsmObjectStoreRequest mObjectStoreRequest;
};