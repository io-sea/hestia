#pragma once

#include "WebApp.h"

namespace hestia {

class DistributedHsmService;

class HestiaWebApp : public WebApp {

  public:
    HestiaWebApp(DistributedHsmService* hestia_service);
};
}  // namespace hestia