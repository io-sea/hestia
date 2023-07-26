#include <catch2/catch_all.hpp>

#include "HestiaCli.h"
#include "MockHestiaClient.h"
#include "StringUtils.h"

#include <iostream>

class TestConsoleInterface : public hestia::IConsoleInterface {
  public:
    void console_write(const std::string& output) const override
    {
        m_output += output;
    }

    void console_write_error(const std::string& output) const override
    {
        m_err_output += output;
    }

    bool console_read(std::string& line) const override
    {
        if (m_input_line_count == m_input_lines.size()) {
            return false;
        }
        line = m_input_lines[m_input_line_count];
        m_input_line_count++;
        return true;
    }
    mutable std::string m_output;
    mutable std::string m_err_output;
    std::vector<std::string> m_input_lines;
    mutable std::size_t m_input_line_count{0};
};

class HestiaCliTestFixture {
  public:
    HestiaCliTestFixture()
    {
        m_test_client = std::make_unique<hestia::mock::MockHestiaClient>();

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
        auto rc = m_cli->run(m_test_client.get());
        REQUIRE(rc.ok());
    }

    std::unique_ptr<hestia::HestiaCli> m_cli;
    TestConsoleInterface* m_console{nullptr};
    std::unique_ptr<hestia::mock::MockHestiaClient> m_test_client;
};

TEST_CASE_METHOD(HestiaCliTestFixture, "Test Hestia CLI - Create", "[hestia]")
{
    WHEN("The default create arg is used")
    {
        std::vector<std::string> args = {"hestia", "object", "create"};
        parse_args(args);
        run();

        THEN("We get just an output id")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE(output_lines.size() == 1);
            REQUIRE(output_lines[0] == "mock_id_0");
        }
    }

    WHEN("The create arg is used with json output")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "--output_fmt=json"};
        parse_args(args);
        run();

        THEN("We get just json attr output")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE(output_lines.size() == 1);
            REQUIRE(output_lines[0] == "my_attr_body");
        }
    }

    WHEN("The create arg is used with key value output")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "--output_fmt=key_value"};
        parse_args(args);
        run();

        THEN("We get just json attr output")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE(output_lines.size() == 1);
            REQUIRE(output_lines[0] == "my_attr_key, my_attr_value");
        }
    }

    WHEN("The create arg is used with key value and id output")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "--output_fmt=id_key_value"};
        parse_args(args);
        run();

        THEN("We get just json attr output")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE(output_lines.size() == 3);
            REQUIRE(output_lines[0] == "mock_id_0");
            REQUIRE(output_lines[2] == "my_attr_key, my_attr_value");
        }
    }

    WHEN("Ids are passed through std in")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "--input_fmt=key_value"};
        m_console->m_input_lines = {"id0", "id1"};

        parse_args(args);
        run();

        THEN("We get the corresponding ids back")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE(output_lines.size() == 2);
            REQUIRE(output_lines[0] == "id0");
            REQUIRE(output_lines[1] == "id1");
        }
    }

    WHEN("An id is passed in the create args body")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "myid123"};
        parse_args(args);
        run();

        THEN("It is returned in the response")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE(output_lines.size() == 1);
            REQUIRE(output_lines[0] == "myid123");
        }
    }
}

TEST_CASE_METHOD(HestiaCliTestFixture, "Test Hestia CLI - Update", "[hestia]")
{
    WHEN("Key value pairs are used for the update")
    {
        std::vector<std::string> args = {
            "hestia", "object", "update", "--input_fmt=key_value",
            "--output_fmt=id_key_value"};
        m_console->m_input_lines = {
            "id0", "mkey0, myval0", "id1", "mykey1, myval1"};

        parse_args(args);
        run();

        THEN("We get ids and key value outputs")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE(output_lines.size() == 7);
            REQUIRE(output_lines[0] == "id0");
            REQUIRE(output_lines[1] == "id1");
            REQUIRE(output_lines[4] == "mkey0, myval0");
            REQUIRE(output_lines[6] == "mykey1, myval1");
        }
    }

    WHEN("Json is used for the update")
    {
        std::vector<std::string> args = {
            "hestia", "object", "update", "--input_fmt=json",
            "--output_fmt=json"};
        m_console->m_input_lines = {"content"};

        parse_args(args);
        run();

        THEN("We get ids and key value outputs")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE(output_lines.size() == 1);
            REQUIRE(output_lines[0] == "content");
        }
    }
}

TEST_CASE_METHOD(HestiaCliTestFixture, "Test Hestia CLI - Read", "[hestia]")
{
    std::vector<std::string> args = {
        "hestia", "object", "read", "--query_fmt=id", "--output_fmt=json",
        "abc"};

    parse_args(args);
    run();

    std::vector<std::string> output_lines;
    hestia::StringUtils::to_lines(m_console->m_output, output_lines);
    REQUIRE(output_lines.size() == 1);
    REQUIRE(output_lines[0] == "query_output");
}