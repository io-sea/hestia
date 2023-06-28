#pragma once

#include "Dictionary.h"
#include "Uuid.h"

#include <ctime>
#include <string>

namespace hestia {
class Model {

  public:
    enum class SerializeFormat { ID, CHILD_ID, FULL };

    Model(const std::string& type);

    Model(const std::string& name, const std::string& type);

    Model(const Uuid& id);

    virtual ~Model();

    virtual void deserialize(
        const Dictionary& dict, SerializeFormat format = SerializeFormat::FULL);

    std::time_t get_last_modified_time() const;

    std::time_t get_creation_time() const;

    const std::string& name() const;

    /**
     * Set the created and last-modified timestamps to current unix time
     */
    void initialize_timestamps();

    const Uuid& id() const;

    void reset_id();

    virtual void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const;

    void set_name(const std::string& name);

    void set_type(const std::string& type);

    virtual void set_id(const Uuid& id);

    virtual void set_creation_time(std::time_t time);

    void set_creation_time(const std::string& time);

    /**
     * Specify the last-modified time since unix epoch
     * @param time the last-modified time
     */
    virtual void update_modified_time(std::time_t time);

    void update_modified_time(const std::string& time);

    /**
     * Set the last-modified time to the current time
     */
    void update_modified_time();

    bool valid() const;

  protected:
    Uuid m_id;
    std::string m_name;
    std::string m_type;
    std::time_t m_creation_time{0};
    std::time_t m_last_modified_time{0};
};
}  // namespace hestia