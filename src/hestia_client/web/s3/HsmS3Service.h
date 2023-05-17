#pragma once

#include "S3Service.h"

namespace hestia {
class HsmService;

class HsmS3Service : public S3Service {
  public:
    HsmS3Service(HsmService* hsm_service);

  private:
    HsmService* m_hsm_service{nullptr};
};
}  // namespace hestia