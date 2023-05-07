#pragma once

#include "User.h"

#include <memory>

namespace hestia {
class Database;

class UserService {
  public:
    using Ptr = std::unique_ptr<UserService>;

    UserService(Database* db);

    virtual ~UserService() = default;

    static Ptr create(Database* db);

    void add(const User& user) const;

    bool exists(const User& user) const;

    virtual void fetch_token(User& user) const;

  private:
    Database* m_db{nullptr};
};
}  // namespace hestia