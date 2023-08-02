#include "HestiaWebApp.h"

#include "CrudWebView.h"
#include "PingView.h"
#include "StaticContentView.h"
#include "TokenAuthenticationMiddleware.h"
#include "UrlRouter.h"

#include "DistributedHsmService.h"
#include "HestiaHsmActionView.h"
#include "HestiaUserAuthView.h"
#include "HsmItem.h"
#include "HsmNode.h"
#include "HsmService.h"
#include "User.h"

#include "HestiaWebPages.h"

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
        {api_prefix + HsmItem::hsm_object_name + "s"},
        std::make_unique<CrudWebView>(
            hestia_service->get_hsm_service(), HsmItem::hsm_object_name));
    m_url_router->add_pattern(
        {api_prefix + HsmItem::dataset_name + "s"},
        std::make_unique<CrudWebView>(
            hestia_service->get_hsm_service(), HsmItem::dataset_name));
    m_url_router->add_pattern(
        {api_prefix + HsmItem::tier_name + "s"},
        std::make_unique<CrudWebView>(
            hestia_service->get_hsm_service(), HsmItem::tier_name));
    m_url_router->add_pattern(
        {api_prefix + HsmItem::tier_extents_name + "s"},
        std::make_unique<CrudWebView>(
            hestia_service->get_hsm_service(), HsmItem::tier_extents_name));
    m_url_router->add_pattern(
        {api_prefix + HsmItem::user_metadata_name + "s"},
        std::make_unique<CrudWebView>(
            hestia_service->get_hsm_service(), HsmItem::user_metadata_name));
    m_url_router->add_pattern(
        {api_prefix + HsmItem::namespace_name + "s"},
        std::make_unique<CrudWebView>(
            hestia_service->get_hsm_service(), HsmItem::namespace_name));
    m_url_router->add_pattern(
        {api_prefix + HsmNode::get_type() + "s"},
        std::make_unique<CrudWebView>(
            hestia_service->get_node_service(), HsmNode::get_type()));
    m_url_router->add_pattern(
        {api_prefix + User::get_type() + "s"},
        std::make_unique<CrudWebView>(
            hestia_service->get_user_service(), User::get_type()));

    m_url_router->add_pattern(
        {api_prefix + HsmItem::hsm_action_name + "s"},
        std::make_unique<HestiaHsmActionView>(hestia_service));

    m_url_router->add_pattern(
        {api_prefix + "register", api_prefix + "login"},
        std::make_unique<HestiaUserAuthView>(m_user_service));

    m_url_router->add_pattern(
        {api_prefix + "ping"}, std::make_unique<PingView>());

    if (!config.m_static_resource_dir.empty()) {
        m_url_router->add_pattern(
            {"/"},
            std::make_unique<StaticContentView>(
                config.m_static_resource_dir, config.m_cache_static_resources));
    }
    else {
        auto index_page = std::make_unique<StaticContentView>();
        index_page->set_buffer(HestiaWebPages::get_default_index_view());
        m_url_router->add_pattern({"/"}, std::move(index_page));
    }

    auto token_auth_middleware =
        std::make_unique<TokenAuthenticationMiddleware>();
    token_auth_middleware->set_user_service(m_user_service);
    add_middleware(std::move(token_auth_middleware));
}
}  // namespace hestia