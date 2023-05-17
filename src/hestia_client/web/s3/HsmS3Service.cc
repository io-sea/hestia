#include "HsmS3Service.h"

namespace hestia {
HsmS3Service::HsmS3Service(HsmService* hsm_service) :
    S3Service({}), m_hsm_service(hsm_service)
{
    (void)m_hsm_service;
}
}  // namespace hestia
