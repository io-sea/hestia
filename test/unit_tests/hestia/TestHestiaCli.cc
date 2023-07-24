#include <catch2/catch_all.hpp>

#include "HestiaCli.h"
#include "HestiaClient.h"

#include <iostream>

class TestConsoleInterface : public hestia::IConsoleInterface {
    void console_write(const std::string& output) const override
    {
        m_last_output = output;
    }

    bool console_read(std::string& line) const override
    {
        (void)line;
        return false;
    }
    mutable std::string m_last_output;
};

class TestHestiaClient :
    public hestia::IHestiaClient,
    public hestia::IHestiaApplication {
  public:
    hestia::OpStatus initialize(
        const std::string& config_path         = {},
        const std::string& user_token          = {},
        const hestia::Dictionary& extra_config = {}) override
    {
        std::cout << "init" << std::endl;
        (void)config_path;
        (void)user_token;
        (void)extra_config;
        return {};
    }

    hestia::OpStatus create(
        const hestia::HestiaType& subject,
        hestia::VecCrudIdentifier& ids,
        hestia::CrudAttributes& attributes,
        hestia::CrudAttributes::Format output_format =
            hestia::CrudAttributes::Format::JSON) override
    {
        (void)subject;
        (void)ids;
        (void)attributes;
        (void)output_format;
        return {};
    }

    hestia::OpStatus read(
        const hestia::HestiaType& subject, hestia::CrudQuery& query) override
    {
        (void)subject;
        (void)query;
        return {};
    }

    hestia::OpStatus update(
        const hestia::HestiaType& subject,
        const hestia::VecCrudIdentifier& ids,
        hestia::CrudAttributes& attributes,
        hestia::CrudAttributes::Format output_format =
            hestia::CrudAttributes::Format::JSON) override
    {
        (void)subject;
        (void)ids;
        (void)attributes;
        (void)output_format;
        return {};
    }

    hestia::OpStatus remove(
        const hestia::HestiaType& subject,
        const hestia::VecCrudIdentifier& ids) override
    {
        (void)subject;
        (void)ids;
        return {};
    }

    hestia::OpStatus do_data_movement_action(hestia::HsmAction& action) override
    {
        (void)action;
        return {};
    }

    void do_data_io_action(
        hestia::HsmAction& action,
        hestia::Stream* stream,
        dataIoCompletionFunc completion_func) override
    {
        (void)action;
        (void)stream;
        (void)completion_func;
    }

    void get_last_error(std::string& error) override { (void)error; }

    void set_last_error(const std::string& msg) override { (void)msg; }

    hestia::OpStatus run() override { return {}; }
};

class HestiaCliTestFixture {
  public:
    HestiaCliTestFixture()
    {
        auto console = std::make_unique<TestConsoleInterface>();
        m_console    = console.get();

        m_cli = std::make_unique<hestia::HestiaCli>(std::move(console));
    }

    void parse_args(const std::vector<std::string>& args)
    {
        char** input_args = new char*[args.size()];
        for (std::size_t idx = 0; idx < args.size(); idx++) {
            const auto arg_size = args[idx].size();
            input_args[idx]     = new char[arg_size + 1];
            for (std::size_t jdx = 0; jdx < arg_size; jdx++) {
                input_args[idx][jdx] = args[idx][jdx];
            }
            input_args[idx][arg_size] = '\0';
        }
        m_cli->parse_args(args.size(), input_args);

        for (std::size_t idx = 0; idx < args.size(); idx++) {
            delete[] input_args[idx];
        }
        delete[] input_args;
    }

    void run()
    {
        auto rc = m_cli->run(&m_test_client);
        REQUIRE(rc.ok());
    }

    std::unique_ptr<hestia::HestiaCli> m_cli;
    TestConsoleInterface* m_console{nullptr};
    TestHestiaClient m_test_client;
};

TEST_CASE_METHOD(HestiaCliTestFixture, "Test Hestia CLI", "[hestia]")
{
    std::vector<std::string> args = {"hestia", "object", "create"};
    parse_args(args);
    // run();
}