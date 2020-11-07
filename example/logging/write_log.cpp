#include <cstdlib>
#include <iostream>

#include <lyra/lyra.hpp>
#include <mprpc/logging/basic_loggers.h>
#include <mprpc/logging/logging_macros.h>

int main(int argc, char* argv[]) {
    try {
        bool show_help = false;
        std::string filename;
        auto cli = lyra::help(show_help) |
            lyra::opt(filename, "filename")["-o"]["--output"](
                "output file (if none, standard output will be used)");

        auto result = cli.parse({argc, argv});

        if (!result) {
            std::cerr << "Error in parsing arguments: " << result.errorMessage()
                      << std::endl;
            return EXIT_FAILURE;
        }

        if (show_help) {
            std::cout << cli << std::endl;
            return EXIT_SUCCESS;
        }

        std::shared_ptr<mprpc::logging::logger> logger;

        if (filename.empty()) {
            logger = mprpc::logging::create_stdout_logger(
                mprpc::logging::log_level::trace);
        } else {
            logger = mprpc::logging::create_file_logger(
                "test_logger", filename, mprpc::logging::log_level::trace);
        }

        MPRPC_TRACE(logger, "test");
        MPRPC_DEBUG(logger, "test");
        MPRPC_INFO(logger, "test");
        MPRPC_WARN(logger, "test");
        MPRPC_ERROR(logger, "test");
        MPRPC_FATAL(logger, "test");

        constexpr int value = 37;
        MPRPC_INFO(logger, "value: {}", value);

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
