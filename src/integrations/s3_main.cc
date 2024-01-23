#include "Logger.h"

#include "CurlClient.h"
#include "S3Client.h"
#include "S3ObjectStoreClient.h"

#include "FileStreamSink.h"
#include "FileStreamSource.h"
#include "JsonDocument.h"

#include <CLI/CLI11.hpp>

#include <filesystem>
#include <future>
#include <iostream>
#include <vector>

std::string get_token(hestia::HttpClient& http_client)
{
    const std::string host = "127.0.0.1:8080";
    const auto endpoint    = host + "/api/v1/login";
    hestia::HttpRequest req(endpoint, hestia::HttpRequest::Method::POST);
    req.set_query("user", "hestia_default_user");
    req.set_query("password", "my_user_pass");

    std::promise<hestia::HttpResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();
    auto completion_func =
        [&response_promise](hestia::HttpResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };
    http_client.make_request(req, completion_func);

    const auto response = response_future.get();
    if (!response->body().empty()) {
        hestia::JsonDocument json(response->body());
        hestia::Dictionary dict;
        json.write(dict);
        return dict.get_map_item("tokens")
            ->get_sequence()[0]
            ->get_map_item("value")
            ->get_scalar();
    }
    return {};
}

int main(int argc, char** argv)
{
    std::string resource_key;
    std::string bucket_key;
    std::string resource_path;
    std::string preferred_node;

    bool activate_logger{false};

    CLI::App app{"Hestia S3 Tool", "hestia_s3_tool"};

    auto put_object = app.add_subcommand("put_object", "Put an object to s3.");

    auto get_object =
        app.add_subcommand("get_object", "Get an object from s3.");

    std::vector<CLI::App*> commands = {put_object, get_object};

    for (auto command : commands) {
        command
            ->add_option("--key", resource_key, "Key for the object or bucket.")
            ->required();
        command
            ->add_option(
                "--file", resource_path, "Path to file with object content.")
            ->required();
        command
            ->add_option(
                "--bucket", bucket_key,
                "Key for the bucket to place in - will be created if needed.")
            ->required();
        command->add_option(
            "--preferred_node", preferred_node,
            "Address of preferred node to send with the action.");
        command->add_option(
            "--activate_logger", activate_logger,
            "Whether to force the hestia logger to active.");
    }

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        app.exit(e);
        throw std::runtime_error(
            "Returning after command line parsing. --help or invalid argument(s)");
    }

    hestia::CurlClient curl_client({});
    const auto token = get_token(curl_client);
    std::cout << "using token " << token << std::endl;

    hestia::S3Client s3_client(&curl_client);

    hestia::S3ObjectStoreClient s3_object_store_client(&s3_client);

    hestia::S3Config s3_config;
    s3_config.set_default_host("127.0.0.1:8090");
    s3_config.set_access_key_id("hestia_default_user");

    hestia::Map extra_headers;
    extra_headers.set_item("hestia-preferred_node", preferred_node);
    s3_config.set_extra_headers(extra_headers);

    s3_object_store_client.do_initialize("1234", "", s3_config);

    auto target_path = std::filesystem::path(resource_path);
    if (target_path.is_relative()) {
        target_path = std::filesystem::current_path() / target_path;
    }

    if (activate_logger) {
        hestia::LoggerConfig logger_config;
        logger_config.set_active(true);
        logger_config.set_level(hestia::LoggerConfig::Level::DEBUG);
        logger_config.set_console_only(true);
        logger_config.set_should_assert_on_error(false);
        hestia::Logger::get_instance().do_initialize({}, logger_config);
    }

    hestia::Stream stream;
    hestia::StorageObject object(resource_key);
    object.get_metadata_as_writeable().set_item(
        "hestia-bucket_name", bucket_key);
    object.get_metadata_as_writeable().set_item(
        "hestia-user_id", "hestia_default_user");
    object.get_metadata_as_writeable().set_item("hestia-user_token", token);

    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_func =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };
    auto progress_func = [](hestia::ObjectStoreResponse::Ptr) {};

    if (put_object->parsed()) {
        stream.set_source(hestia::FileStreamSource::create(target_path));

        hestia::ObjectStoreContext ctx(
            hestia::ObjectStoreRequest(
                object, hestia::ObjectStoreRequestMethod::PUT),
            completion_func, progress_func, &stream);
        s3_object_store_client.make_request(ctx);
        const auto response = response_future.get();
        if (!response->ok()) {
            LOG_ERROR("Error in s3 put");
        }
    }
    else if (get_object->parsed()) {
        stream.set_sink(hestia::FileStreamSink::create(target_path));

        hestia::ObjectStoreContext ctx(
            hestia::ObjectStoreRequest(
                object, hestia::ObjectStoreRequestMethod::GET),
            completion_func, progress_func, &stream);
        s3_object_store_client.make_request(ctx);
        const auto response = response_future.get();
        if (!response->ok()) {
            LOG_ERROR("Error in s3 get");
        }
    }
    return 0;
}