#include "TimeProvider.h"

#include "TimeUtils.h"

namespace hestia {
std::time_t DefaultTimeProvider::get_current_time() const
{
    return TimeUtils::get_current_time();
}
}  // namespace hestia
