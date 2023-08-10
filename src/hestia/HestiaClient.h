#pragma once

#include "HestiaApplication.h"
#include "HestiaCommands.h"
#include "Stream.h"

#include <mutex>
#include <thread>
#include <unordered_map>

namespace hestia {

class IHestiaClient : public IHestiaApplication {
  public:
    virtual ~IHestiaClient() = default;

    virtual OpStatus create(
        const HestiaType& subject,
        VecCrudIdentifier& ids,
        CrudAttributes& attributes,
        CrudAttributes::Format output_format =
            CrudAttributes::Format::JSON) = 0;

    virtual OpStatus read(const HestiaType& subject, CrudQuery& query) = 0;

    virtual OpStatus update(
        const HestiaType& subject,
        const VecCrudIdentifier& id,
        CrudAttributes& attributes,
        CrudAttributes::Format output_format =
            CrudAttributes::Format::JSON) = 0;

    virtual OpStatus remove(
        const HestiaType& subject, const VecCrudIdentifier& id) = 0;

    virtual OpStatus do_data_movement_action(HsmAction& action) = 0;

    using dataIoCompletionFunc =
        std::function<void(OpStatus status, const HsmAction& action)>;
    virtual void do_data_io_action(
        const HsmAction& action,
        Stream* stream,
        dataIoCompletionFunc completion_func) = 0;

    virtual void get_last_error(std::string& error) = 0;

    virtual void set_last_error(const std::string& msg) = 0;
};

class HestiaClient : public IHestiaClient, public HestiaApplication {
  public:
    HestiaClient();

    ~HestiaClient();

    OpStatus initialize(
        const std::string& config_path = {},
        const std::string& user_token  = {},
        const Dictionary& extra_config = {},
        const std::string& server_host = {},
        unsigned server_port           = 8080) override;

    OpStatus create(
        const HestiaType& subject,
        VecCrudIdentifier& ids,
        CrudAttributes& attributes,
        CrudAttributes::Format output_format =
            CrudAttributes::Format::JSON) override;

    OpStatus read(const HestiaType& subject, CrudQuery& query) override;

    OpStatus update(
        const HestiaType& subject,
        const VecCrudIdentifier& id,
        CrudAttributes& attributes,
        CrudAttributes::Format output_format =
            CrudAttributes::Format::JSON) override;

    OpStatus remove(
        const HestiaType& subject, const VecCrudIdentifier& id) override;

    OpStatus do_data_movement_action(HsmAction& action) override;

    void do_data_io_action(
        const HsmAction& action,
        Stream* stream,
        dataIoCompletionFunc completion_func) override;

    void get_last_error(std::string& error) override;

    void set_last_error(const std::string& msg) override;

    OpStatus run() override { return {}; }

    std::string get_runtime_info() const override;

  private:
    void clear_last_error();

    void set_app_mode(const std::string& host, unsigned port) override;

    void load_object_store_defaults();

    mutable std::mutex m_mutex;
    std::unordered_map<std::thread::id, std::string> m_last_errors;
};
}  // namespace hestia