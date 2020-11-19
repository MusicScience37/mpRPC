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
 * \brief server for test of echo
 */
#include <atomic>
#include <csignal>
#include <cstdio>
#include <memory>
#include <thread>

#include <fmt/core.h>
#include <lyra/lyra.hpp>

#include "echo_test.h"
#include "mprpc/server_builder.h"

std::shared_ptr<mprpc::logging::logger> get_logger() {
    static const auto logger =
        mprpc::logging::create_stdout_logger(mprpc::logging::log_level::warn);
    return logger;
}

std::string echo(std::string str) { return str; }

void create_server(const std::string& host, const std::string& type) {
    static std::unique_ptr<mprpc::server> echo_server;
    static const auto logger = get_logger();

    echo_server.reset();
    const auto cooling_time = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(cooling_time);

    mprpc::server_builder builder{get_logger()};
    builder.num_threads(2);
    builder.method<std::string(std::string)>("echo", echo);
    if (type == "tcp") {
        builder.listen_tcp(host, echo_server_port);
    } else if (type == "tcp_zstd") {
        builder.listen_tcp(host, echo_server_port, "zstd");
    } else if (type == "udp") {
        builder.listen_udp(host, echo_server_port);
    } else if (type == "udp_zstd") {
        builder.listen_udp(host, echo_server_port, "zstd");
    } else {
        throw std::runtime_error("unkown type " + type);
    }
    echo_server = builder.create();

    std::this_thread::sleep_for(cooling_time);
}

// signal handling
volatile std::atomic<bool> is_enabled{true};
extern "C" void handle_signal(int /*signal*/);
void handle_signal(int /*signal*/) { is_enabled = false; }

void run(const std::string& host) {
    auto inst_server =
        mprpc::server_builder(get_logger())
            .listen_tcp(host, inst_server_port)
            .method<void(std::string)>("create_server",
                [host](const std::string& type) { create_server(host, type); })
            .create();

    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);
    while (is_enabled) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main(int argc, char** argv) {
    std::string host = "127.0.0.1";
    const auto cli = lyra::cli() |
        lyra::opt(host, "host")["--host"]["-H"]("set host address of server");
    const auto cli_result = cli.parse({argc, argv});
    if (!cli_result) {
        std::cerr << "Error in parsing arguments: " << cli_result.errorMessage()
                  << std::endl;
        return 1;
    }

    try {
        run(host);
        std::cout << "server normally exiting" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        fmt::print("Error: {}\n", e.what());
        std::fflush(stdout);
        return 1;
    }
}
