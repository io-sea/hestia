#include "Datamover.h"

#include "Logger.h"
#include "StringUtils.h"

#include "hestia_iosea.h"

namespace hestia {
Datamover::Datamover(const std::string& host)
{
    if (host.empty()) {
        hestia_initialize(nullptr, nullptr, nullptr);
    }
    else {
        std::string config =
            "{\"server\" : {\"controller_address\" : \"" + host + "\"}}";
        std::cout << "starting with config: " << config << std::endl;
        hestia_initialize(nullptr, nullptr, config.c_str());
    }
}

Datamover::~Datamover()
{
    hestia_finish();
}

void Datamover::apply_dataset_command(
    const std::string& dataset_id, const std::string& command)
{
    HestiaId id;
    hestia_init_id(&id);

    StringUtils::to_char(dataset_id, &id.m_uuid);

    HestiaObject object;
    auto rc = hestia_object_get_attrs(&id, &object);
    if (rc != 0) {
        LOG_ERROR("Error getting object attributes: " + dataset_id);
        return;
    }

    std::vector<char> payload(object.m_size);

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
    io_ctx.m_buffer = payload.data();
    io_ctx.m_length = payload.size();

    HestiaTier tier;

    hestia_object_get(&id, &io_ctx, &tier);

    hestia_init_object(&object);
    delete[] id.m_uuid;

    std::string payload_str(payload.begin(), payload.end());

    std::vector<std::string> lines;
    StringUtils::to_lines(payload_str, lines);
    for (const auto& line : lines) {
        const auto& [object_id, _] = StringUtils::split_on_first(line, ",");
        if (object_id.size() < 36) {
            continue;
        }
        apply_object_command(object_id, command);
    }
}

void Datamover::apply_object_command(
    const std::string& object_id, const std::string& command)
{
    HestiaId id;
    hestia_init_id(&id);

    StringUtils::to_char(object_id, &id.m_uuid);

    const auto& [command_key, command_val] =
        StringUtils::split_on_first(command, "=");

    auto kv_pairs = new HestiaKeyValuePair[1];
    StringUtils::to_char(command_key, &kv_pairs[0].m_key);
    StringUtils::to_char(command_val, &kv_pairs[0].m_value);
    auto rc = hestia_object_set_attrs(&id, kv_pairs, 1);
    if (rc != 0) {
        LOG_ERROR("Error getting object attributes: " + object_id);
        return;
    }

    delete[] kv_pairs[0].m_key;
    delete[] kv_pairs[0].m_value;
    delete[] kv_pairs;
}
}  // namespace hestia