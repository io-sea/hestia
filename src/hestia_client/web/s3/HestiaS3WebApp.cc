#include "HestiaS3WebApp.h"

#include "S3ContainerListView.h"
#include "S3ContainerView.h"
#include "S3ObjectView.h"

#include "S3UrlRouter.h"

#include "HsmS3Service.h"

namespace hestia {
HestiaS3WebApp::HestiaS3WebApp(HsmS3Service* hsm_s3_service)
{
    m_url_router = std::make_unique<hestia::S3UrlRouter>();
    m_url_router->add_pattern(
        "/", std::make_unique<S3ContainerListView>(hsm_s3_service));
}
};  // namespace hestia