#pragma once

#include <memory>

class HsmService;

class ApplicationContext {
  public:
    static ApplicationContext& get();

    HsmService* getHsmService() const;

    void setHsmService(std::unique_ptr<HsmService> hsmService);

  private:
    std::unique_ptr<HsmService> mHsmService;
};