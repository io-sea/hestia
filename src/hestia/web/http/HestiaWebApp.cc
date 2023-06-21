#include "HestiaWebApp.h"

#include "HestiaDatasetView.h"
#include "HestiaNodeView.h"
#include "HestiaObjectView.h"
#include "HestiaTierView.h"
#include "HestiaUserAuthView.h"
#include "HestiaUserView.h"

#include "StaticContentView.h"
#include "TokenAuthenticationMiddleware.h"
#include "UrlRouter.h"

#include "Logger.h"

namespace hestia {
HestiaWebApp::HestiaWebApp(
    UserService* user_service,
    DistributedHsmService* hestia_service,
    HestiaWebAppConfig config) :
    WebApp(user_service)
{
    LOG_INFO("Creating HestiaWebApp");
    const std::string api_prefix = "/api/v1/";

    m_url_router = std::make_unique<hestia::UrlRouter>();
    m_url_router->add_pattern(
        {api_prefix + "hsm/objects"},
        std::make_unique<HestiaObjectView>(hestia_service));
    m_url_router->add_pattern(
        {api_prefix + "hsm/tiers"},
        std::make_unique<HestiaTierView>(hestia_service));
    m_url_router->add_pattern(
        {api_prefix + "hsm/datasets"},
        std::make_unique<HestiaDatasetView>(hestia_service));
    m_url_router->add_pattern(
        {api_prefix + "nodes"},
        std::make_unique<HestiaNodeView>(hestia_service));
    m_url_router->add_pattern(
        {api_prefix + "users"},
        std::make_unique<HestiaUserView>(m_user_service));
    m_url_router->add_pattern(
        {api_prefix + "register", api_prefix + "login"},
        std::make_unique<HestiaUserAuthView>(m_user_service));

    if (!config.m_static_resource_dir.empty()) {
        m_url_router->add_pattern(
            {"/"},
            std::make_unique<StaticContentView>(
                config.m_static_resource_dir, config.m_cache_static_resources));
    }

    auto token_auth_middleware =
        std::make_unique<TokenAuthenticationMiddleware>();
    token_auth_middleware->set_user_service(m_user_service);
    add_middleware(std::move(token_auth_middleware));
}
}  // namespace hestia