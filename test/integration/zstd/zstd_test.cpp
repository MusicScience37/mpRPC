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
 * \brief test of RPC using zstd compression
 */
#include <catch2/catch_test_macros.hpp>

#include "mprpc/client_builder.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/method_client.h"
#include "mprpc/server_builder.h"
#include "mprpc/transport/udp/udp_acceptor_config.h"
#include "mprpc/transport/udp/udp_connector_config.h"

namespace {

auto create_logger() {
    static constexpr std::size_t max_file_size = 1024 * 1024;
    static constexpr std::size_t max_files = 5;
    static const auto logger = mprpc::logging::create_file_logger(
        "mprpc_test_integ_zstd", "mprpc_test_integ_zstd.log",
        mprpc::logging::log_level::trace, max_file_size, max_files, true);
    return logger;
}

const auto host = "127.0.0.1";
constexpr std::uint16_t port = 3780;

}  // namespace

TEST_CASE("RPC on UDP using zstd compression") {
    const auto logger = create_logger();

    constexpr std::size_t line_length = 100;
    logger->write(
        mprpc::logging::log_level::info, std::string(line_length, '-'));

    auto server = mprpc::server_builder(logger)
                      .num_threads(2)
                      .listen_udp(host, port, "zstd")
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .create();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    auto client = mprpc::client_builder(logger)
                      .num_threads(2)
                      .connect_udp(host, port, "zstd")
                      .create();

    auto echo_client =
        mprpc::method_client<std::string(std::string)>(*client, "echo");

    SECTION("request echo") {
        const auto str = std::string("abc");
        REQUIRE(echo_client(str) == str);
    }

    SECTION("request echo with large data") {
        // larger than UDP packet limit
        constexpr std::size_t size = 1024 * 1024;
        const auto str = std::string(size, 'a');
        std::string res;
        REQUIRE_NOTHROW(res = echo_client(str));
        // prevent large output on failure
        REQUIRE(str.size() == res.size());
        REQUIRE(std::equal(str.begin(), str.end(), res.begin(), res.end()));
    }
}

TEST_CASE("RPC on TCP using zstd compression") {
    const auto logger = create_logger();

    constexpr std::size_t line_length = 100;
    logger->write(
        mprpc::logging::log_level::info, std::string(line_length, '-'));

    auto server = mprpc::server_builder(logger)
                      .num_threads(2)
                      .listen_tcp(host, port, "zstd")
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .create();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    auto client = mprpc::client_builder(logger)
                      .num_threads(2)
                      .connect_tcp(host, port, "zstd")
                      .create();

    auto echo_client =
        mprpc::method_client<std::string(std::string)>(*client, "echo");

    SECTION("request echo") {
        const auto str = std::string("abc");
        REQUIRE(echo_client(str) == str);
    }

    SECTION("request echo with large data") {
        constexpr std::size_t size = 1024 * 1024;
        const auto str = std::string(size, 'a');
        std::string res;
        REQUIRE_NOTHROW(res = echo_client(str));
        // prevent large output on failure
        REQUIRE(str.size() == res.size());
        REQUIRE(std::equal(str.begin(), str.end(), res.begin(), res.end()));
    }
}
