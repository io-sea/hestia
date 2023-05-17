#pragma once

#include "WebApp.h"

namespace hestia {

class HestiaService;

class HestiaWebApp : public WebApp {

  public:
    HestiaWebApp(HestiaService* hestia_service);
};
}  // namespace hestia