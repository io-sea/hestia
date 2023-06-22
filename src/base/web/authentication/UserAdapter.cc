#include "UserAdapter.h"

#include "JsonUtils.h"
#include "StringUtils.h"
#include "Uuid.h"

#include "Logger.h"

namespace hestia {

UserJsonAdapter::UserJsonAdapter(bool return_tokens) :
    m_return_tokens(return_tokens)
{
}

void UserJsonAdapter::to_string(
    const std::vector<User>& items, std::string& output) const
{
    auto seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& item : items) {
        auto dict = std::make_unique<Dictionary>();
        to_dict(item, *dict);
        seq->add_sequence_item(std::move(dict));
    }
    output = JsonUtils::to_json(*seq);
    LOG_INFO("Saving as: " << output);
}

void UserJsonAdapter::to_dict(
    const User& item, Dictionary& dict, const std::string& id) const
{
    std::string working_id;
    if (!id.empty()) {
        working_id = id;
    }
    else {
        working_id = item.id();
    }

    std::unordered_map<std::string, std::string> data{
        {"id", working_id},
        {"password", item.m_password},
        {"last_modified", std::to_string(item.m_last_modified_time)},
        {"created", std::to_string(item.m_creation_time)}};
    dict.set_map(data);

    if (m_return_tokens) {
        auto token_dict = std::make_unique<Dictionary>();

        std::unordered_map<std::string, std::string> token_data{
            {"value", item.m_api_token.m_value},
            {"created", std::to_string(item.m_api_token.m_creation_time)}};
        token_dict->set_map(token_data);
        dict.set_map_item("token", std::move(token_dict));
    }
}

bool UserJsonAdapter::matches_query(
    const User& item, const Metadata& query) const
{
    LOG_INFO("Checking query: " << query.to_string());
    if (auto token = query.get_item("token::value"); !token.empty()) {
        LOG_INFO("Item value is: " << item.m_api_token.m_value);
        if (item.m_api_token.m_value != token) {
            return false;
        }
    }
    return true;
}

void UserJsonAdapter::from_dict(const Dictionary& dict, User& item) const
{
    Metadata scalar_data;
    dict.get_map_items(scalar_data);

    auto on_item = [&item](const std::string& key, const std::string& value) {
        if (key == "last_modified" && !value.empty()) {
            item.m_last_modified_time = std::stoull(value);
        }
        else if (key == "created" && !value.empty()) {
            item.m_creation_time = std::stoull(value);
        }
        else if (key == "password" && !value.empty()) {
            item.m_password = value;
        }
        else if (key == "id" && !value.empty()) {
            if (item.m_identifier.empty()) {
                item.m_identifier = value;
            }
        }
    };
    scalar_data.for_each_item(on_item);

    if (dict.has_map_item("token")) {
        Metadata token_data;
        dict.get_map_item("token")->get_map_items(token_data);
        auto on_token_item =
            [&item](const std::string& key, const std::string& value) {
                if (key == "created" && !value.empty()) {
                    item.m_api_token.m_creation_time = std::stoull(value);
                }
                else if (key == "value" && !value.empty()) {
                    item.m_api_token.m_value = value;
                }
            };
        token_data.for_each_item(on_token_item);
    }
}

void UserJsonAdapter::to_string(
    const User& item, std::string& output, const std::string& id) const
{
    Dictionary dict;
    to_dict(item, dict, id);
    output = JsonUtils::to_json(dict);
}

void UserJsonAdapter::from_string(const std::string& input, User& item) const
{
    LOG_INFO("Loading as: " << input);
    auto dict = JsonUtils::from_json(input);
    from_dict(*dict, item);
}

void UserJsonAdapter::from_string(
    const std::string& json, std::vector<User>& items) const
{
    auto dict = JsonUtils::from_json(json);
    if (dict->get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& entry : dict->get_sequence()) {
            items.emplace_back(User());
            from_dict(*entry, items[items.size() - 1]);
        }
    }
}

}  // namespace hestia
