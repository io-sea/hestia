#pragma once

#include "HsmObjectStoreClient.h"
#include "MotrConfig.h"

#include <memory>

namespace hestia {
class MotrInterface;

class MotrHsmClient : public HsmObjectStoreClient {
  public:
    MotrHsmClient(std::unique_ptr<MotrInterface> interface = nullptr);

    void do_intialize(
        const std::string& id,
        const std::string& cache_path,
        const MotrConfig& config);

    void initialize(
        const std::string& id,
        const std::string& cache_path,
        const Dictionary& config) override;

    void set_tier_ids(const std::vector<std::string>& tier_ids) override;

    virtual ~MotrHsmClient(){};

    void copy(HsmObjectStoreContext& ctx) const override;

    void get(HsmObjectStoreContext& ctx) const override;

    void move(HsmObjectStoreContext& ctx) const override;

    void put(HsmObjectStoreContext& ctx) const override;

    void remove(HsmObjectStoreContext& ctx) const override;

  protected:
    std::unique_ptr<MotrInterface> m_motr_interface;
};
}  // namespace hestia