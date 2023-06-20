#include "S3WebApp.h"

#include "S3AuthenticationMiddleware.h"
#include "S3ContainerListView.h"
#include "S3ContainerView.h"
#include "S3ObjectView.h"
#include "S3Service.h"
#include "S3UrlRouter.h"

#include "ObjectStoreClient.h"

#include <filesystem>

namespace hestia {

S3WebApp::S3WebApp(
    ObjectStoreClient* object_store,
    UserService* user_service,
    const S3WebAppConfig& config,
    S3Service::Ptr s3_service) :
    WebApp(user_service)
{
    (void)config;
    m_object_store_client = object_store;

    if (s3_service) {
        m_s3_service = std::move(s3_service);
    }
    else {
        S3ServiceConfig service_config;
        service_config.m_object_store_client = m_object_store_client;
        m_s3_service = std::make_unique<S3Service>(service_config);
    }

    set_up_routing();

    set_up_middleware();
}

void S3WebApp::set_up_routing()
{
    auto s3_router = std::make_unique<S3UrlRouter>();

    s3_router->set_object_view(
        std::make_unique<S3ObjectView>(m_s3_service.get()));
    s3_router->set_container_view(
        std::make_unique<S3ContainerView>(m_s3_service.get()));
    s3_router->set_container_list_view(
        std::make_unique<S3ContainerListView>(m_s3_service.get()));

    m_url_router = std::move(s3_router);
}

void S3WebApp::set_up_middleware()
{
    auto auth_middleware = std::make_unique<S3AuthenticationMiddleware>();
    add_middleware(std::move(auth_middleware));
}
}  // namespace hestia