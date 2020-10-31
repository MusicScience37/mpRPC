#include <iostream>
#include <limits>
#include <string>

#include <lyra/lyra.hpp>

#include "mprpc/logging/logging_macros.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/server_builder.h"

int main(int argc, char* argv[]) {
    try {
        bool show_help = false;
        bool verbose = false;

        std::string host = "127.0.0.1";
        constexpr std::uint16_t default_port = 3780;
        std::uint16_t port = default_port;

        const auto cli = lyra::help(show_help) |
            lyra::opt(verbose)["--verbose"]["-v"]("verbose logging") |
            lyra::opt(host, "address")["--host"]("set host IP address") |
            lyra::opt(port, "port")["--port"]["-p"]("set port number");

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

        const auto server = mprpc::server_builder(logger)
                                .listen_tcp(host, port)
                                .method<std::string(std::string)>(
                                    "echo", [](std::string str) { return str; })
                                .create();

        MPRPC_INFO(logger, "press enter to exit");
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
