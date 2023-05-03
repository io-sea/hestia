#include "UserService.h"

#include "Database.h"

#include <fstream>
#include <stdexcept>

namespace hestia {
UserService::UserService(Database* db) : m_db(db) {}

UserService::Ptr UserService::create(Database* db)
{
    return std::make_unique<UserService>(db);
}

void UserService::add(const User& user) const {}

bool UserService::exists(const User& user) const
{
    return false;
}

void UserService::fetch_token(User& user) const
{
    std::ifstream user_database("/tmp/users.txt");
    if (!user_database.good()) {
        throw std::runtime_error("db is corrupted");
    }
    std::string user_str;
    while (user_database >> user_str) {
        int colon = user_str.find(':');
        if (colon == std::string::npos) {
            throw std::runtime_error("db is corrupted");
        }
        std::string user_id = user_str.substr(0, colon);
        if (user_id == user.m_identifier) {
            user.m_token = user_str.substr(colon + 1, user_str.size());
            break;
        }
    }
}
}  // namespace hestia