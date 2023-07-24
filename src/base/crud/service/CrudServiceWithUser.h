#pragma once

#include "CrudService.h"
#include "UserService.h"

namespace hestia {

class CrudClient;
class IdGenerator;
class TimeProvider;

class CrudServiceWithUser : public CrudService {
  public:
    CrudServiceWithUser(
        const ServiceConfig& config,
        std::unique_ptr<CrudClient> client,
        UserService* user_service,
        std::unique_ptr<IdGenerator> id_generator = nullptr);

  protected:
    UserService* m_user_service{nullptr};
};
}  // namespace hestia