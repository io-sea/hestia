#pragma once

#include "CrudService.h"
#include "WebApp.h"

namespace hestia::mock {
class MockCrudWebApp : public WebApp {
  public:
    MockCrudWebApp(CrudService* crud_service);
    using Ptr = std::unique_ptr<MockCrudWebApp>;

    std::string m_api_prefix{"/api/v2/abc"};
};
}  // namespace hestia::mock