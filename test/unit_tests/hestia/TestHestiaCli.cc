#include <catch2/catch_all.hpp>

#include "HestiaCli.h"
#include "HestiaClient.h"
#include "StringUtils.h"
#include "TestClientConfigs.h"

#include <iostream>

class TestConsoleInterface : public hestia::IConsoleInterface {
  public:
    void console_write(const std::string& output) const override
    {
        m_output += output + "\n";
    }

    void console_write_error(const std::string& output) const override
    {
        m_err_output += output;
    }

    void console_read(std::string& buffer) const override { buffer = m_input; }

    void console_write(const std::vector<std::string>& output) const override
    {
        for (const auto& line : output) {
            m_output += line + '\n';
        }
    }

    mutable std::string m_output;
    mutable std::string m_err_output;
    std::string m_input;
};

class HestiaCliTestFixture {
  public:
    HestiaCliTestFixture()
    {
        hestia::Dictionary extra_config;
        hestia::TestClientConfigs::get_hsm_memory_client_config(extra_config);

        m_test_client = std::make_unique<hestia::HestiaClient>();
        m_test_client->initialize({}, {}, extra_config);

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

    void run(bool skip_init = false)
    {
        auto rc = m_cli->run(m_test_client.get(), skip_init);
        REQUIRE(rc.ok());
    }

    std::unique_ptr<hestia::HestiaCli> m_cli;
    TestConsoleInterface* m_console{nullptr};
    std::unique_ptr<hestia::HestiaApplication> m_test_client;
};

TEST_CASE_METHOD(HestiaCliTestFixture, "Test Hestia CLI - Create", "[hestia]")
{
    WHEN("The default create arg is used")
    {
        std::vector<std::string> args = {"hestia", "object", "create"};
        parse_args(args);
        run();

        THEN("We get some output - which should be an id")
        {
            REQUIRE_FALSE(m_console->m_output.empty());
        }
    }

    WHEN("The create arg is used with an id")
    {
        std::vector<std::string> args = {"hestia", "object", "create", "1234"};
        parse_args(args);
        run();

        THEN("We get just an output id")
        {
            std::vector<std::string> output_lines;
            hestia::StringUtils::to_lines(m_console->m_output, output_lines);
            REQUIRE_FALSE(output_lines.empty());
            REQUIRE(output_lines[0] == "1234");
        }
    }

    WHEN("The create arg is used with json output")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "1234", "--output_fmt=json"};
        parse_args(args);
        run();

        THEN("We get json attr output")
        {
            REQUIRE_FALSE(m_console->m_output.empty());
            hestia::Dictionary dict;
            hestia::JsonDocument(m_console->m_output).write(dict);
            REQUIRE(dict.has_map_item("id"));
            REQUIRE(dict.get_map_item("id")->get_scalar() == "1234");
        }
    }

    WHEN("The create arg is used with key value output")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "1234", "--output_fmt=key_value"};
        parse_args(args);
        run();

        THEN("We get dict attr output")
        {
            REQUIRE_FALSE(m_console->m_output.empty());
            hestia::Dictionary dict;
            dict.from_string(m_console->m_output);
            REQUIRE(dict.has_map_item("id"));
            REQUIRE(dict.get_map_item("id")->get_scalar() == "1234");
        }
    }

    WHEN(" We create with multiple ids passed through std in")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "--input_fmt=id"};
        m_console->m_input = "id0\nid1";

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

    WHEN(" We create with kv pairs and an id through std in")
    {
        std::vector<std::string> args = {
            "hestia", "object", "create", "--input_fmt=key_value",
            "--output_fmt=key_value"};
        m_console->m_input = R"(
            name=my_object
            id=1234
        )";

        parse_args(args);
        run();

        THEN("We get the kv pairs and id in the output")
        {
            REQUIRE_FALSE(m_console->m_output.empty());

            hestia::Dictionary::FormatSpec dict_format;
            hestia::Dictionary dict(m_console->m_output, dict_format);

            REQUIRE(dict.has_map_item("id"));
            REQUIRE(dict.get_map_item("id")->get_scalar() == "1234");
            REQUIRE(dict.has_map_item("name"));
            REQUIRE(dict.get_map_item("name")->get_scalar() == "my_object");
        }
    }
}

TEST_CASE_METHOD(HestiaCliTestFixture, "Test Hestia CLI - Update", "[hestia]")
{
    std::vector<std::string> create_args = {
        "hestia", "object", "create", "1234"};
    parse_args(create_args);
    run();
    m_console->m_output.clear();

    /*
    WHEN("Key value pairs are used for the update")
    {
        std::vector<std::string> args = {
            "hestia", "metadata", "update", "1234",
            "--id_fmt=parent_id",
            "--input_fmt=key_value",
            "--output_fmt=key_value"};
        m_console->m_input = "data.my_key0=my_value0\ndata.my_key1=my_value1";

        parse_args(args);
        run(true);

        THEN("We get ids and key value outputs")
        {
            REQUIRE_FALSE(m_console->m_output.empty());
            hestia::Dictionary dict;
            dict.from_string(m_console->m_output);
            REQUIRE(dict.has_map_item("data"));
            REQUIRE_FALSE(dict.get_map_item("data")->is_empty());
        }
    }
    */

    WHEN("Json is used for the update")
    {
        std::vector<std::string> args = {"hestia",
                                         "metadata",
                                         "update",
                                         "1234",
                                         "--id_fmt=parent_id",
                                         "--input_fmt=json",
                                         "--output_fmt=json"};
        m_console->m_input            = R"(
            {"data" : { "my_key0" : "my_value0",
                        "my_key1" : "my_value1"}}
                        )";

        parse_args(args);
        run(true);

        THEN("We get json outputs")
        {
            REQUIRE_FALSE(m_console->m_output.empty());
            hestia::Dictionary dict;
            hestia::JsonDocument(m_console->m_output).write(dict);
            REQUIRE(dict.has_map_item("data"));
            REQUIRE_FALSE(dict.get_map_item("data")->is_empty());
        }
    }
}

/*
TEST_CASE_METHOD(HestiaCliTestFixture, "Test Hestia CLI - Read", "[hestia]")
{
    std::vector<std::string> args = {
        "hestia", "object", "read", "--query_fmt=id", "--output_fmt=json",
        "abc"};

    parse_args(args);
    run();

    std::vector<std::string> output_lines;
    hestia::StringUtils::to_lines(m_console->m_output, output_lines);
    REQUIRE_FALSE(output_lines.empty());
}
*/