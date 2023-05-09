#pragma once

#include <string>

struct m0_client;

struct ClientConfig {
    std::string mLocalAddress;
    std::string mHaAddress;
    std::string mProfile;
    std::string mProcFid;
};

class MotrInterface {
  public:
    virtual ~MotrInterface() = default;
    virtual void initializeClientInstance(const ClientConfig& config);

    void initializeHsm();

    m0_client* getClientInstance() const;

  protected:
    m0_client* mClientInstance{nullptr};
    ClientConfig mClientConfig;
};