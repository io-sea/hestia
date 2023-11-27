#include "Datamover.h"

#include "hestia_iosea.h"

namespace hestia {
Datamover::Datamover()
{
    hestia_initialize(nullptr, nullptr, nullptr);
}

Datamover::~Datamover()
{
    hestia_finish();
}

void Datamover::apply_dataset_command(
    const std::string& dataset_name, const std::string& command)
{
    (void)dataset_name;
    (void)command;
}

void Datamover::apply_object_command(
    const std::string& id, const std::string& command)
{
    (void)id;
    (void)command;
}
}  // namespace hestia