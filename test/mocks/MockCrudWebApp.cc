#include "MockCrudWebApp.h"

#include "MockCrudView.h"

#include "UrlRouter.h"

namespace hestia::mock {
MockCrudWebApp::MockCrudWebApp(CrudService* crud_service) : WebApp(nullptr)
{
    m_url_router = std::make_unique<hestia::UrlRouter>();

    m_url_router->add_pattern(
        {m_api_prefix + "/" + crud_service->get_type() + "s"},
        std::make_unique<MockCrudView>(crud_service));
}

}  // namespace hestia::mock