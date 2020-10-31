#include <cstdlib>
#include <future>
#include <iostream>
#include <utility>
#include <vector>

#include <lyra/lyra.hpp>
#include <msgpack.hpp>

#include "mprpc/error_info.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/logging/spdlog_logger.h"

void write_with_log_levels(
    const std::shared_ptr<mprpc::logging::logger>& logger) {
#line 1 "test.cpp"
    MPRPC_TRACE(logger, "test trace");
    MPRPC_DEBUG(logger, "test debug");
    MPRPC_INFO(logger, "test info");
    MPRPC_WARN(logger, "test warn");
    MPRPC_ERROR(logger, "test error");
    MPRPC_FATAL(logger, "test fatal");
}

void write_with_data(const std::shared_ptr<mprpc::logging::logger>& logger) {
    constexpr int value1 = 37;
    constexpr float value2 = 1.5F;
#line 10 "test.cpp"
    MPRPC_INFO(logger, "value: {}, {:.2}", value1, value2);
}

void write_error(const std::shared_ptr<mprpc::logging::logger>& logger) {
    msgpack::sbuffer stream;
    msgpack::pack(stream,
        std::make_tuple(std::string("abc"),
            2.5F,                                                  // NOLINT
            std::vector<char>{char(0xA9), char(0xF1), char(0x3C)}  // NOLINT
            ));
    const auto data = mprpc::message_data(stream.data(), stream.size());

    constexpr mprpc::error_code_t code = 37;
    const auto info = mprpc::error_info(code, "test error", data);

#line 20 "test.cpp"
    MPRPC_WARN(logger, "{}", info);
}

void write_in_thread(const std::shared_ptr<mprpc::logging::logger>& logger) {
    constexpr std::size_t threads = 3;
    std::array<std::future<void>, threads> future{};

    for (std::size_t i = 0; i < threads; ++i) {
        future[i] = std::async(std::launch::async, [logger, i] {
#line 30 "test.cpp"
            MPRPC_INFO(logger, "test logger in a thread {}", i);
        });
    }

    for (std::size_t i = 0; i < threads; ++i) {
        future[i].get();
    }
}

int main(int argc, char* argv[]) {
    try {
        bool show_help = false;
        std::string filename;
        bool verbose = false;
        auto cli = lyra::help(show_help) |
            lyra::opt(filename, "filename")["-l"]["--log"](
                "output file (if none, standard output will be used)") |
            lyra::opt(verbose)["-v"]["--verbose"]("verbose output");

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

        auto log_output_level = mprpc::logging::log_level::info;
        if (verbose) {
            log_output_level = mprpc::logging::log_level::trace;
        }

        if (filename.empty()) {
            logger = mprpc::logging::create_stdout_logger(log_output_level);
        } else {
            logger = mprpc::logging::create_file_logger(
                "test_logger", filename, log_output_level);
        }

        write_with_log_levels(logger);
        write_with_data(logger);
        write_error(logger);
        write_in_thread(logger);

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
