#include "Model.h"

#include "TimeUtils.h"
#include "UuidUtils.h"

namespace hestia {

Model::Model(const std::string& type) : m_type(type)
{
    m_creation_time      = TimeUtils::get_current_time();
    m_last_modified_time = m_creation_time;
}

Model::Model(const std::string& name, const std::string& type) :
    m_name(name), m_type(type)
{
    m_creation_time      = TimeUtils::get_current_time();
    m_last_modified_time = m_creation_time;
}

Model::Model(const Uuid& id) : m_id(id)
{
    m_creation_time      = TimeUtils::get_current_time();
    m_last_modified_time = m_creation_time;
}

Model::~Model() {}

void Model::deserialize(const Dictionary& dict, SerializeFormat format)
{
    auto on_item = [this,
                    format](const std::string& key, const std::string& value) {
        if (key == "id" && !value.empty() && m_id.is_unset()) {
            m_id = UuidUtils::from_string(value);
        }

        if (format != SerializeFormat::ID) {
            if (key == "name" && !value.empty()) {
                m_name = value;
            }
            else if (key == "type" && !value.empty()) {
                m_type = value;
            }
            else if (key == "creation_time" && !value.empty()) {
                m_creation_time = std::stoull(value);
            }
            else if (key == "last_modified_time" && !value.empty()) {
                m_last_modified_time = std::stoull(value);
            }
        }
    };
    dict.for_each_scalar(on_item);
};

void Model::serialize(
    Dictionary& dict, SerializeFormat format, const Uuid& id) const
{
    Uuid working_id;
    if (!id.is_unset()) {
        working_id = id;
    }
    else {
        working_id = m_id;
    }

    std::unordered_map<std::string, std::string> data;
    data["id"] = UuidUtils::to_string(working_id);

    if (format != SerializeFormat::ID) {
        data["name"]               = m_name;
        data["type"]               = m_type;
        data["creation_time"]      = std::to_string(m_creation_time);
        data["last_modified_time"] = std::to_string(m_last_modified_time);
    }
    dict.set_map(data);
}

void Model::reset_id()
{
    m_id = Uuid();
}

std::time_t Model::get_last_modified_time() const
{
    return m_last_modified_time;
}

std::time_t Model::get_creation_time() const
{
    return m_creation_time;
}

const Uuid& Model::id() const
{
    return m_id;
}

const std::string& Model::name() const
{
    return m_name;
}

void Model::set_name(const std::string& name)
{
    m_name = name;
}

void Model::set_type(const std::string& type)
{
    m_type = type;
}

void Model::set_id(const Uuid& id)
{
    m_id = id;
}

void Model::update_modified_time(std::time_t time)
{
    m_last_modified_time = time;
}

void Model::update_modified_time()
{
    m_last_modified_time = TimeUtils::get_current_time();
}

void Model::initialize_timestamps()
{
    m_creation_time = TimeUtils::get_current_time();
    update_modified_time(m_creation_time);
}

bool Model::valid() const
{
    return !(m_id.is_unset() || m_name.empty());
}

void Model::update_modified_time(const std::string& time)
{
    update_modified_time(std::stoull(time));
}

void Model::set_creation_time(const std::string& time)
{
    set_creation_time(std::stoull(time));
}

void Model::set_creation_time(std::time_t time)
{
    m_creation_time = time;
}

}  // namespace hestia