#include "CrudServiceWithUser.h"

#include "CrudClient.h"
#include "IdGenerator.h"
#include "TimeProvider.h"

namespace hestia {
CrudServiceWithUser::CrudServiceWithUser(
    const ServiceConfig& config,
    std::unique_ptr<CrudClient> client,
    UserService* user_service,
    std::unique_ptr<IdGenerator> id_generator) :
    CrudService(config, std::move(client), std::move(id_generator)),
    m_user_service(user_service)
{
}

}  // namespace hestia