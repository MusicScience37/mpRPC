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
 * \brief test of echo server
 */
#include <functional>
#include <iostream>
#include <utility>

#include <lyra/lyra.hpp>

#include "mprpc/client_builder.h"
#include "mprpc/generate_string.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/method_client.h"
#include "mprpc/server_builder.h"
#include "mprpc/stop_watch.h"

static void echo_tcp(std::size_t num_repetitions, std::size_t data_size,
    mprpc::stop_watch& watch) {
    static const auto logger =
        mprpc::logging::create_stdout_logger(mprpc::logging::log_level::warn);

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    const auto data = mprpc::generate_string(data_size);

    auto server = mprpc::server_builder(logger)
                      .num_threads(2)
                      .listen_tcp(host, port)
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .create();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    auto client = mprpc::client_builder(logger)
                      .num_threads(2)
                      .connect_tcp(host, port)
                      .create();

    auto echo_client =
        mprpc::method_client<std::string(std::string)>(*client, "echo");

    for (std::size_t i = 0; i < num_repetitions; ++i) {
        watch.start();
        echo_client(data);
        watch.stop();
    }
}

static void echo_tcp_zstd(std::size_t num_repetitions, std::size_t data_size,
    mprpc::stop_watch& watch) {
    static const auto logger =
        mprpc::logging::create_stdout_logger(mprpc::logging::log_level::warn);

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    const auto data = mprpc::generate_string(data_size);

    auto server = mprpc::server_builder(logger)
                      .num_threads(2)
                      .use_zstd_compression()
                      .listen_tcp(host, port)
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .create();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    auto client = mprpc::client_builder(logger)
                      .num_threads(2)
                      .use_zstd_compression()
                      .connect_tcp(host, port)
                      .create();

    auto echo_client =
        mprpc::method_client<std::string(std::string)>(*client, "echo");

    for (std::size_t i = 0; i < num_repetitions; ++i) {
        watch.start();
        echo_client(data);
        watch.stop();
    }
}

static void echo_udp(std::size_t num_repetitions, std::size_t data_size,
    mprpc::stop_watch& watch) {
    static const auto logger =
        mprpc::logging::create_stdout_logger(mprpc::logging::log_level::warn);

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    const auto data = mprpc::generate_string(data_size);

    auto server = mprpc::server_builder(logger)
                      .num_threads(2)
                      .listen_udp(host, port)
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .create();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    auto client = mprpc::client_builder(logger)
                      .num_threads(2)
                      .connect_udp(host, port)
                      .create();

    auto echo_client =
        mprpc::method_client<std::string(std::string)>(*client, "echo");

    for (std::size_t i = 0; i < num_repetitions; ++i) {
        watch.start();
        echo_client(data);
        watch.stop();
    }
}

static void echo_udp_zstd(std::size_t num_repetitions, std::size_t data_size,
    mprpc::stop_watch& watch) {
    static const auto logger =
        mprpc::logging::create_stdout_logger(mprpc::logging::log_level::warn);

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    const auto data = mprpc::generate_string(data_size);

    auto server = mprpc::server_builder(logger)
                      .num_threads(2)
                      .use_zstd_compression()
                      .listen_udp(host, port)
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .create();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    auto client = mprpc::client_builder(logger)
                      .num_threads(2)
                      .use_zstd_compression()
                      .connect_udp(host, port)
                      .create();

    auto echo_client =
        mprpc::method_client<std::string(std::string)>(*client, "echo");

    for (std::size_t i = 0; i < num_repetitions; ++i) {
        watch.start();
        echo_client(data);
        watch.stop();
    }
}

int main(int argc, char** argv) {
    constexpr std::size_t default_num_repetitions = 100;
    std::size_t num_repetitions = default_num_repetitions;
    const auto cli = lyra::cli() |
        lyra::opt(num_repetitions, "number")["--repetitions"]["-r"](
            "set number of repetitions");
    const auto cli_result = cli.parse({argc, argv});
    if (!cli_result) {
        std::cerr << "Error in parsing arguments: " << cli_result.errorMessage()
                  << std::endl;
        return 1;
    }

    const auto data_sizes =
        std::vector<std::size_t>{1, 32, 1024, 1024 * 32, 1024 * 1024};
    const auto benchmarks = std::vector<std::pair<std::string,
        std::function<void(std::size_t, std::size_t, mprpc::stop_watch&)>>>{
        {"echo_tcp", echo_tcp}, {"echo_tcp_zstd", echo_tcp_zstd},
        {"echo_udp", echo_udp}, {"echo_udp_zstd", echo_udp_zstd}};

    for (const auto& benchmark : benchmarks) {
        for (const auto data_size : data_sizes) {
            try {
                const auto name =
                    fmt::format("{}_{}", benchmark.first, data_size);
                fmt::print("performing benchmark {}\n", name);
                std::fflush(stdout);

                mprpc::stop_watch watch;
                watch.reserve(num_repetitions);
                benchmark.second(num_repetitions, data_size, watch);
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
