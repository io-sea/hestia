#pragma once

#include <memory>
#include <string>

namespace hestia {
class DistributedHsmService;
class HsmService;
class S3ObjectAdapter;
class S3DatasetAdapter;

class S3Service {
  public:
    struct Status {
        enum class Code { OK, ERROR };

        Status() = default;

        Status(Code code, const std::string& message) :
            m_code(code), m_message(message)
        {
        }

        bool ok() const { return m_code == Code::OK; }

        Code m_code{Code::OK};
        std::string m_message;
    };

    S3Service(DistributedHsmService* hsm_service);

    virtual ~S3Service();

    HsmService* get_hsm_service() const;

  private:
    DistributedHsmService* m_hsm_service{nullptr};
    std::unique_ptr<S3ObjectAdapter> m_object_adapter;
    std::unique_ptr<S3DatasetAdapter> m_dataset_adapter;
};
}  // namespace hestia