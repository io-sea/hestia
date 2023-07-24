#pragma once

#include "HsmItem.h"
#include <string>

namespace hestia {

class StorageTier : public HsmItem, public Model {
  public:
    StorageTier();

    StorageTier(const std::string& id);

    StorageTier(uint8_t id);

    StorageTier(const StorageTier& other);

    static std::string get_type();

    const std::string& get_backend() const;

    std::size_t get_capacity() const;

    std::size_t get_bandwidth() const;

    uint8_t id_uint() const;

    void set_backend(const std::string& backend);

    void set_capacity(std::size_t capacity);

    void set_bandwidth(std::size_t bandwidth);

    StorageTier& operator=(const StorageTier& other);

  private:
    void init();

    UIntegerField m_capacity{"capacity"};
    UIntegerField m_bandwidth{"bandwidth"};
    StringField m_backend{"backend"};
};
}  // namespace hestia
