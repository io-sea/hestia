#pragma once

#include "S3Service.h"

namespace hestia {
class HsmService;
class KeyValueStoreClient;

class HsmS3Service : public S3Service {
  public:
    HsmS3Service(HsmService* hsm_service, KeyValueStoreClient* kv_store_client);

    virtual ~HsmS3Service();

    [[nodiscard]] std::pair<Status, bool> exists(
        const S3Container& container) const noexcept override;

    [[nodiscard]] Status get(S3Container& container) const noexcept override;

    [[nodiscard]] Status list(std::vector<S3Container>& fetched) const override;

    [[nodiscard]] Status list(
        const S3Container& container,
        std::vector<S3Object>& fetched) const override;

    [[nodiscard]] Status put(const S3Container& container) noexcept override;

  private:
    std::pair<Status, bool> exists(const std::string& object_id) const override;

    HsmService* m_hsm_service{nullptr};
    KeyValueStoreClient* m_kv_store_client{nullptr};
};
}  // namespace hestia