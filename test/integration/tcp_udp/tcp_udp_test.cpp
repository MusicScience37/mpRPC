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
 * \brief test of RPC on TCP and UDP
 */
#include <catch2/catch_test_macros.hpp>

#include "mprpc/client_builder.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/method_client.h"
#include "mprpc/server_builder.h"

TEST_CASE("RPC on TCP") {
    static constexpr std::size_t max_file_size = 1024 * 1024;
    static constexpr std::size_t max_files = 5;
    static const auto logger =
        mprpc::logging::create_file_logger("mprpc_test_integ_tcp_udp.log",
            mprpc::logging::log_level::trace, max_file_size, max_files, true);

    auto server = mprpc::server_builder(logger)
                      .num_threads(2)
                      .listen_tcp()
                      .listen_udp()
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .create();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    SECTION("TCP client") {
        auto client =
            mprpc::client_builder(logger).num_threads(2).connect_tcp().create();

        auto echo_client =
            mprpc::method_client<std::string(std::string)>(client, "echo");

        const auto timeout = std::chrono::seconds(3);

        const auto str = std::string("abc");
        auto future = echo_client.async_request(str).get_future();
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(future.get() == str);
    }

    SECTION("UDP client") {
        auto client =
            mprpc::client_builder(logger).num_threads(2).connect_udp().create();

        auto echo_client =
            mprpc::method_client<std::string(std::string)>(client, "echo");

        const auto timeout = std::chrono::seconds(3);
        const auto str = std::string("abc");
        auto future = echo_client.async_request(str).get_future();
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(future.get() == str);
    }

    server->stop();
    std::this_thread::sleep_for(duration);
}
