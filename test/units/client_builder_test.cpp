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
 * \brief test of client_builder class
 */
#include "mprpc/client_builder.h"

#include <catch2/catch_test_macros.hpp>

#include "create_logger.h"
#include "mprpc/server_builder.h"

TEST_CASE("mprpc::client_builder") {
    auto logger = create_logger("mprpc::client_builder");

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    auto server = mprpc::server_builder(logger)
                      .num_threads(1)
                      .listen_tcp(host, port)
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .create();

    const auto wait_duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(wait_duration);

    SECTION("create a clinet") {
        auto client = mprpc::client_builder(logger)
                          .num_threads(2)
                          .connect_tcp(host, port)
                          .create();

        const auto str = std::string("abcde");
        auto future =
            client->async_request<std::string>("echo", str).get_future();

        const auto timeout = std::chrono::seconds(3);
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(future.get() == str);
    }
}
