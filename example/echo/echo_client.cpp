#include <iostream>
#include <limits>
#include <string>

#include <lyra/lyra.hpp>

#include "mprpc/client_builder.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/method_client.h"

int main(int argc, char* argv[]) {
    try {
        bool show_help = false;
        bool verbose = false;

        std::string host = "127.0.0.1";
        constexpr std::uint16_t default_port = 3780;
        std::uint16_t port = default_port;

        std::string data = "abc";

        const auto cli = lyra::help(show_help) |
            lyra::opt(verbose)["--verbose"]["-v"]("verbose logging") |
            lyra::opt(host, "address")["--host"]("set host IP address") |
            lyra::opt(port, "port")["--port"]["-p"]("set port number") |
            lyra::opt(data, "data")["--data"]["-d"]("set data");

        const auto result = cli.parse({argc, argv});

        if (!result) {
            std::cerr << "Error in parsing arguments: " << result.errorMessage()
                      << std::endl;
            return 1;
        }

        if (show_help) {
            std::cout << cli << std::endl;
            return 0;
        }

        const auto logger = mprpc::logging::create_stdout_logger(verbose
                ? mprpc::logging::log_level::trace
                : mprpc::logging::log_level::info);

        const auto client =
            mprpc::client_builder(logger).connect_tcp(host, port).create();

        auto echo =
            mprpc::method_client<std::string(std::string)>(*client, "echo");

        MPRPC_INFO(logger, "send {}", data);
        const auto response = echo(data);
        MPRPC_INFO(logger, "received {}", response);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
