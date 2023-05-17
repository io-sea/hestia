#pragma once

#include "WebApp.h"

namespace hestia {

class HsmS3Service;

class HestiaS3WebApp : public WebApp {
  public:
    HestiaS3WebApp(HsmS3Service*);
};
}  // namespace hestia