#include "HestiaS3WebApp.h"

#include "S3ContainerListView.h"
#include "S3ContainerView.h"
#include "S3ObjectView.h"

#include "S3AuthenticationMiddleware.h"
#include "S3UrlRouter.h"

#include "UserService.h"

#include "HsmS3Service.h"

namespace hestia {
HestiaS3WebApp::HestiaS3WebApp(
    HestiaS3WebAppConfig config,
    HsmS3Service* s3_service,
    UserService* user_service) :
    WebApp(user_service), m_config(config), m_s3_service(s3_service)
{
    (void)m_config;
    set_up_routing();

    set_up_middleware();
}

void HestiaS3WebApp::set_up_routing()
{
    auto s3_router = std::make_unique<S3UrlRouter>();

    s3_router->set_object_view(std::make_unique<S3ObjectView>(m_s3_service));
    s3_router->set_container_view(
        std::make_unique<S3ContainerView>(m_s3_service));
    s3_router->set_container_list_view(
        std::make_unique<S3ContainerListView>(m_s3_service));

    m_url_router = std::move(s3_router);
}

void HestiaS3WebApp::set_up_middleware()
{
    auto auth_middleware = std::make_unique<S3AuthenticationMiddleware>();
}
}  // namespace hestia