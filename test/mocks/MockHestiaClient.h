#pragma once

#include "HestiaApplication.h"
#include "HestiaClient.h"
#include "Logger.h"

#include <unordered_map>

namespace hestia::mock {
class MockHestiaClient : public IHestiaClient {
  public:
    MockHestiaClient();

    virtual ~MockHestiaClient() {}

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

    OpStatus update(
        const HestiaType& subject,
        const VecCrudIdentifier& ids,
        CrudAttributes& attributes,
        CrudAttributes::Format output_format =
            CrudAttributes::Format::JSON) override;

    OpStatus read(const HestiaType& subject, CrudQuery& query) override;

    OpStatus remove(
        const HestiaType& subject, const VecCrudIdentifier& ids) override;

    OpStatus do_data_movement_action(HsmAction& action) override;

    void do_data_io_action(
        const HsmAction& action,
        Stream* stream,
        dataIoCompletionFunc completion_func) override;

    void get_last_error(std::string& error) override;

    void set_last_error(const std::string& msg) override;

    std::string get_runtime_info() const override;

    OpStatus run() override;

    std::size_t m_created_id_count{0};
    std::vector<std::string> m_created_ids;

    std::unordered_map<uint8_t, std::string> m_buffer;
};
}  // namespace hestia::mock