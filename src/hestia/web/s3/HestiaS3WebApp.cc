#include "HestiaS3WebApp.h"

#include "S3BucketListView.h"
#include "S3BucketView.h"
#include "S3ObjectView.h"

#include "S3AuthenticationMiddleware.h"
#include "S3UrlRouter.h"

#include "UserService.h"

#include "DistributedHsmService.h"

namespace hestia {
HestiaS3WebApp::HestiaS3WebApp(
    HestiaS3WebAppConfig config,
    DistributedHsmService* hsm_service,
    UserService* user_service) :
    WebApp(user_service), m_config(config), m_hsm_service(hsm_service)
{
    set_up_routing();
    set_up_middleware();
}

void HestiaS3WebApp::set_up_routing()
{
    auto s3_router = std::make_unique<S3UrlRouter>();

    s3_router->set_object_view(
        std::make_unique<S3ObjectView>(m_hsm_service, m_config.m_domain));
    s3_router->set_bucket_view(
        std::make_unique<S3BucketView>(m_hsm_service, m_config.m_domain));
    s3_router->set_bucket_list_view(
        std::make_unique<S3BucketListView>(m_hsm_service, m_config.m_domain));

    m_url_router = std::move(s3_router);
}

void HestiaS3WebApp::set_up_middleware()
{
    auto auth_middleware =
        std::make_unique<S3AuthenticationMiddleware>(m_config.m_domain);
    auth_middleware->set_user_service(m_user_service);
    add_middleware(std::move(auth_middleware));
}
}  // namespace hestia