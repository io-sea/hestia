#include "HestiaWebApp.h"

#include "HestiaNodeView.h"
#include "HestiaObjectView.h"
#include "HestiaTierView.h"
#include "UrlRouter.h"

namespace hestia {
HestiaWebApp::HestiaWebApp(HestiaService* hestia_service) : WebApp()
{
    const std::string api_prefix = "/api/v1/";

    m_url_router = std::make_unique<hestia::UrlRouter>();
    m_url_router->add_pattern(
        api_prefix + "hsm/objects/",
        std::make_unique<HestiaObjectView>(hestia_service));
    m_url_router->add_pattern(
        api_prefix + "hsm/tiers/",
        std::make_unique<HestiaTierView>(hestia_service));
    m_url_router->add_pattern(
        api_prefix + "nodes/",
        std::make_unique<HestiaNodeView>(hestia_service));
}
}  // namespace hestia