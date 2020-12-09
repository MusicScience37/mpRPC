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
 * \brief test of RPC on UDP
 */
#include <catch2/catch_test_macros.hpp>

#include "mprpc/client_builder.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/method_client.h"
#include "mprpc/server_builder.h"

TEST_CASE("RPC on UDP") {
    static constexpr std::size_t max_file_size = 1024 * 1024;
    static constexpr std::size_t max_files = 5;
    static const auto logger =
        mprpc::logging::create_file_logger("mprpc_test_integ_udp.log",
            mprpc::logging::log_level::trace, max_file_size, max_files, true);

    std::atomic<int> counter{0};

    auto server = mprpc::server_builder(logger)
                      .num_threads(2)
                      .listen_udp()
                      .method<std::string(std::string)>(
                          "echo", [](std::string str) { return str; })
                      .method<void()>("count", [&counter] { ++counter; })
                      .create();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    auto client =
        mprpc::client_builder(logger).num_threads(2).connect_udp().create();

    auto echo_client =
        mprpc::method_client<std::string(std::string)>(client, "echo");
    auto count_client = mprpc::method_client<void()>(client, "count");

    const auto timeout = std::chrono::seconds(3);

    SECTION("async_request echo") {
        const auto str = std::string("abc");
        auto future = echo_client.async_request(str).get_future();
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(future.get() == str);
    }

    SECTION("request echo") {
        const auto str = std::string("abc");
        REQUIRE(echo_client.request(str) == str);
    }

    SECTION("request echo with operator") {
        const auto str = std::string("abc");
        REQUIRE(echo_client(str) == str);
    }

    SECTION("async_request count") {
        counter = 0;
        auto future = count_client.async_request().get_future();
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE_NOTHROW(future.get());
        REQUIRE(counter.load() == 1);
    }

    SECTION("request count") {
        counter = 0;
        REQUIRE_NOTHROW(count_client.request());
        REQUIRE(counter.load() == 1);
    }

    SECTION("request count with operator") {
        counter = 0;
        REQUIRE_NOTHROW(count_client());
        REQUIRE(counter.load() == 1);
    }

    SECTION("notify count") {
        counter = 0;
        REQUIRE_NOTHROW(count_client.notify());
        constexpr std::size_t repetition = 100;
        for (std::size_t i = 0; i < repetition; ++i) {
            const auto duration = std::chrono::milliseconds(100);
            std::this_thread::sleep_for(duration);
            if (counter == 1) {
                break;
            }
        }
        REQUIRE(counter.load() == 1);
    }

    client->stop();
    server->stop();
    std::this_thread::sleep_for(duration);
}
