#pragma once

#include "Request.h"
#include "RequestError.h"
#include "RequestException.h"
#include "Response.h"

#include "Logger.h"

#include <memory>
#include <string>

namespace hestia {

template<typename RequestT, typename ResponseT, typename ErrorCodeT>
class Service {
  public:
    Service() = default;

    virtual ~Service() = default;

    [[nodiscard]] virtual std::unique_ptr<ResponseT> make_request(
        const RequestT& request) const noexcept = 0;

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