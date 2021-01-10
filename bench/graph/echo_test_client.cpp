/*
 * Copyright 2020 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief client for test of echo
 */
#include <fmt/core.h>
#include <lyra/lyra.hpp>

#include "echo_test.h"
#include "mprpc/client_builder.h"
#include "mprpc/generate_string.h"
#include "mprpc/method_client.h"
#include "mprpc/stop_watch.h"

std::shared_ptr<mprpc::logging::logger> get_logger() {
    static const auto logger =
        mprpc::logging::create_stdout_logger(mprpc::logging::log_level::warn);
    return logger;
}

void test(std::size_t num_repetitions, std::size_t data_size,
    mprpc::stop_watch& watch, const std::string& host,
    const std::string& type) {
    const auto data = mprpc::generate_string(data_size);

    mprpc::client_builder builder{get_logger()};
    builder.num_threads(2);
    if (type == "tcp") {
        builder.connect_tcp(host, echo_server_port);
    } else if (type == "tcp_zstd") {
        builder.connect_tcp(
            host, echo_server_port, mprpc::transport::compression_type::zstd);
    } else if (type == "udp") {
        builder.connect_udp(host, echo_server_port);
    } else if (type == "udp_zstd") {
        builder.connect_udp(
            host, echo_server_port, mprpc::transport::compression_type::zstd);
    } else {
        throw std::runtime_error("unkown type " + type);
    }
    const auto client = builder.create();

    auto echo = mprpc::method_client<std::string(std::string)>(client, "echo");

    for (std::size_t i = 0; i < num_repetitions; ++i) {
        watch.start();
        echo(data);
        watch.stop();
    }
}

int main(int argc, char** argv) {
    constexpr std::size_t default_num_repetitions = 100;
    std::size_t num_repetitions = default_num_repetitions;
    std::string host = "127.0.0.1";
    const auto cli =
        lyra::opt(num_repetitions, "number")["--repetitions"]["-r"](
            "set number of repetitions") |
        lyra::opt(host, "host")["--host"]["-H"]("set host address of server");
    const auto cli_result = cli.parse({argc, argv});
    if (!cli_result) {
        std::cerr << "Error in parsing arguments: " << cli_result.errorMessage()
                  << std::endl;
        return 1;
    }

    const auto data_sizes =
        std::vector<std::size_t>{1, 32, 1024, 1024 * 32, 1024 * 1024};
    const auto benchmarks =
        std::vector<std::string>{"tcp", "tcp_zstd", "udp", "udp_zstd"};

    const auto inst_client = mprpc::client_builder(get_logger())
                                 .connect_tcp(host, inst_server_port)
                                 .create();
    auto create_server =
        mprpc::method_client<void(std::string)>(inst_client, "create_server");

    for (const auto& type : benchmarks) {
        for (const auto data_size : data_sizes) {
            try {
                const auto name = fmt::format("echo_{}_{}", type, data_size);
                fmt::print("performing benchmark {}\n", name);
                std::fflush(stdout);

                create_server(type);

                mprpc::stop_watch watch;
                watch.reserve(num_repetitions);
                test(num_repetitions, data_size, watch, host, type);
                watch.write(fmt::format("{}.txt", name));

                fmt::print("  done\n");
                std::fflush(stdout);
            } catch (const std::exception& e) {
                fmt::print("  error: {}\n", e.what());
                std::fflush(stdout);
            }
        }
    }

    return 0;
}
