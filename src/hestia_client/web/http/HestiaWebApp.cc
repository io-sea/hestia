#include "HestiaWebApp.h"

#include "HestiaWebView.h"
#include "UrlRouter.h"

namespace hestia {
HestiaWebApp::HestiaWebApp(HestiaService* hestia_service) : WebApp()
{
    const std::string api_prefix = "/api/v1/";

    m_url_router = std::make_unique<hestia::UrlRouter>();
    m_url_router->add_pattern(
        api_prefix + "hsm", std::make_unique<HestiaWebView>(hestia_service));
}
}  // namespace hestia