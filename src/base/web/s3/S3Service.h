#pragma once

#include "ObjectStoreClient.h"
#include "S3Container.h"
#include "S3Object.h"
#include "Stream.h"

namespace hestia {
class S3ObjectAdapter;
class S3ContainerAdapter;

struct S3ServiceConfig {
    std::string m_metadata_prefix;
    ObjectStoreClient* m_object_store_client{nullptr};
};

class S3Service {
  public:
    using Ptr = std::unique_ptr<S3Service>;

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

    S3Service(const S3ServiceConfig& config);

    virtual ~S3Service();

    static std::unique_ptr<S3Service> create(const S3ServiceConfig& config);

    [[nodiscard]] virtual std::pair<Status, bool> exists(
        const S3Container& container) const noexcept;

    [[nodiscard]] std::pair<Status, bool> exists(
        const S3Object& object) const noexcept;

    [[nodiscard]] virtual Status get(S3Container& container) const noexcept;

    [[nodiscard]] Status get(
        S3Object& object,
        const Extent& extent = {},
        Stream* stream       = nullptr) const noexcept;

    [[nodiscard]] virtual Status list(std::vector<S3Container>& fetched) const;

    [[nodiscard]] virtual Status list(
        const S3Container& container, std::vector<S3Object>& fetched) const;

    [[nodiscard]] virtual Status put(const S3Container& container) noexcept;

    [[nodiscard]] Status put(
        const S3Container& container,
        const S3Object& object,
        const Extent& extent = {},
        Stream* stream       = nullptr) noexcept;

    [[nodiscard]] Status remove(const S3Container& container) noexcept;

    [[nodiscard]] Status remove(const S3Object& object) noexcept;

  protected:
    virtual std::pair<Status, bool> exists(const std::string& object_id) const;

    std::unique_ptr<S3ObjectAdapter> m_object_adapter;
    std::unique_ptr<S3ContainerAdapter> m_container_adapter;

  private:
    Status remove(const std::string& object_id);

    S3Service::Status make_object_store_request(
        const ObjectStoreRequest& request, Stream* stream = nullptr);

    ObjectStoreClient* m_object_store_client{nullptr};
};
}  // namespace hestia