#pragma once

#include "Dictionary.h"
#include "StringAdapter.h"
#include "User.h"

#include <memory>
#include <sstream>

namespace hestia {

class UserJsonAdapter : public StringAdapter<User> {
  public:
    UserJsonAdapter(bool return_tokens = false);

    void to_string(
        const User& item,
        std::string& output,
        const std::string& id = {}) const override;

    void to_string(
        const std::vector<User>& items, std::string& output) const override;

    void from_string(const std::string& output, User& item) const override;

    void from_string(
        const std::string& json, std::vector<User>& item) const override;

    bool matches_query(const User& item, const Metadata& query) const override
    {
        (void)item;
        (void)query;
        return true;
    };

  private:
    void to_dict(
        const User& node, Dictionary& dict, const std::string& id = {}) const;

    void from_dict(const Dictionary& dict, User& node) const;

    bool m_return_tokens{false};
};
}  // namespace hestia