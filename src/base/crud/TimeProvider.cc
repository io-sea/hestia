#include "TimeProvider.h"

#include "TimeUtils.h"

namespace hestia {
std::chrono::microseconds DefaultTimeProvider::get_current_time() const
{
    return TimeUtils::get_time_since_epoch_micros();
}
}  // namespace hestia
