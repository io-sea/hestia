#pragma once

#include "HestiaApplication.h"
#include "HestiaCommands.h"
#include "HestiaRequest.h"
#include "HestiaResponse.h"
#include "Stream.h"

#include <mutex>
#include <thread>
#include <unordered_map>

namespace hestia {

class IHestiaClient : public IHestiaApplication {
  public:
    virtual ~IHestiaClient() = default;

    using progressFunc = std::function<void(const HsmAction& action)>;

    using completionFunc = std::function<void(HestiaResponse::Ptr response)>;

    virtual void make_request(
        const HestiaRequest& request,
        completionFunc completion_func,
        Stream* stream             = nullptr,
        progressFunc progress_func = nullptr) noexcept = 0;

    virtual void get_last_error(std::string& error) = 0;

    virtual void set_last_error(const std::string& msg) = 0;

    virtual bool is_id_validation_active() const = 0;
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

    void make_request(
        const HestiaRequest& request,
        completionFunc completion_func,
        Stream* stream             = nullptr,
        progressFunc progress_func = nullptr) noexcept override;

    void get_last_error(std::string& error) override;

    void set_last_error(const std::string& msg) override;

    OpStatus run() override { return {}; }

    std::string get_runtime_info() const override;

    bool is_id_validation_active() const override;

  private:
    HestiaResponse::Ptr do_crud(const HestiaRequest& request);

    void do_hsm_action(
        const HestiaRequest& request,
        completionFunc completion_func,
        Stream* stream             = nullptr,
        progressFunc progress_func = nullptr);

    void clear_last_error();

    void set_app_mode(const std::string& host, unsigned port) override;

    void load_object_store_defaults();

    mutable std::mutex m_mutex;
    std::unordered_map<std::thread::id, std::string> m_last_errors;
};
}  // namespace hestia