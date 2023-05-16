#include "MockMotrInterfaceImpl.h"
#include "MotrHsmClient.h"
#include "MotrInterface.h"

namespace hestia::mock {
class MockMotrHsmClient : public hestia::MotrHsmClient {
  public:
    MockMotrHsmClient();

    virtual ~MockMotrHsmClient(){};
};
}  // namespace hestia::mock