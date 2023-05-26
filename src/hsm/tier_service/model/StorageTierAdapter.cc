#include "StorageTierAdapter.h"

namespace hestia {
void StorageTierJsonAdapter::to_string(
    const StorageTier& item, std::string& output) const
{
    (void)item;
    (void)output;
}

void StorageTierJsonAdapter::from_string(
    const std::string& output, StorageTier& item) const
{
    (void)item;
    (void)output;
}
}  // namespace hestia