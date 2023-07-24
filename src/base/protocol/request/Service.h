#pragma once

#include "Request.h"
#include "RequestError.h"
#include "RequestException.h"
#include "Response.h"

#include "Logger.h"

#include <memory>
#include <string>

namespace hestia {

struct ServiceConfig {
    std::string m_endpoint;
    std::string m_global_prefix;
    std::string m_item_prefix;
};

template<typename RequestT, typename ResponseT, typename ErrorCodeT>
class Service {
  public:
    Service(const ServiceConfig& config) : m_config(config) {}

    virtual ~Service() = default;

    [[nodiscard]] virtual std::unique_ptr<ResponseT> make_request(
        const RequestT& request,
        const std::string& type = {}) const noexcept = 0;

  protected:
    ServiceConfig m_config;

  private:
    virtual void on_exception(
        const RequestT& request,
        ResponseT* response,
        const std::string& message = {}) const = 0;

    virtual void on_exception(
        const RequestT& request,
        ResponseT* response,
        const RequestError<ErrorCodeT>& error) const = 0;
};

}  // namespace hestia