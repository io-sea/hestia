#include "StorageTierAdapter.h"

#include "JsonUtils.h"
#include "Metadata.h"

namespace hestia {
void StorageTierJsonAdapter::to_string(
    const StorageTier& item, std::string& output) const
{
    Metadata data;
    data.set_item("backend", item.m_backend);
    output = JsonUtils::to_json(data);
}

void StorageTierJsonAdapter::from_string(
    const std::string& output, StorageTier& item) const
{
    Metadata data;
    JsonUtils::from_json(output, data);

    auto each_item = [&item](const std::string& key, const std::string& value) {
        if (key == "backend") {
            item.m_backend = value;
        }
    };
    data.for_each_item(each_item);
}
}  // namespace hestia