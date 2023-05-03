#pragma once

#include <memory>

namespace hestia {
class HsmService;

class ApplicationContext {
  public:
    static ApplicationContext& get();

    HsmService* get_hsm_service() const;

    void set_hsm_service(std::unique_ptr<HsmService> hsm_service);

  private:
    std::unique_ptr<HsmService> m_hsm_service;
};
}  // namespace hestia