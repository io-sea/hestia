#include "MockMotrClient.h"

namespace hestia::mock {
MockMotrHsmClient::MockMotrHsmClient() :
    MotrHsmClient(std::make_unique<hestia::MotrInterface>(
        std::make_unique<MockMotrInterfaceImpl>()))
{
}

}  // namespace hestia::mock