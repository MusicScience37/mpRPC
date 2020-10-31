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
 * \brief test of client class
 */
#include "mprpc/client.h"

#include <future>

#include <catch2/catch_test_macros.hpp>

#include "create_logger.h"
#include "mprpc/transport/mock/mock_connector.h"

TEST_CASE("mprpc::client") {
    const auto logger = create_logger("mprpc::client");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
    threads->start();

    const auto connector =
        std::make_shared<mprpc::transport::mock::mock_connector>(
            threads->context());

    SECTION("async_request") {
        mprpc::client client{logger, threads, connector};
        client.start();

        std::future<int> future;
        REQUIRE_NOTHROW(
            future = client.async_request<int>("test", 1, 2, 3).get_future());

        const auto request_data = connector->get_written_data();
        mprpc::message request;
        REQUIRE_NOTHROW(request = mprpc::message(request_data));
        REQUIRE(request.method() == "test");
        REQUIRE(request.params_as<std::tuple<int, int, int>>() ==
            std::make_tuple(1, 2, 3));

        constexpr int result = 37;
        const auto response_data =
            mprpc::pack_response(request.msgid(), result);
        connector->add_read_data(response_data);

        const auto timeout = std::chrono::seconds(3);
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(future.get() == result);
    }

    SECTION("request") {
        mprpc::client client{logger, threads, connector};
        client.start();

        auto future = std::async(
            [&client] { return client.request<int>("test", 1, 2, 3); });

        const auto duration = std::chrono::milliseconds(100);
        constexpr std::size_t repetitions = 100;
        mprpc::message_data request_data;
        for (std::size_t i = 0; i < repetitions; ++i) {
            request_data = connector->get_written_data();
            if (request_data.size() > 0) {
                break;
            }
            std::this_thread::sleep_for(duration);
        }

        mprpc::message request;
        REQUIRE_NOTHROW(request = mprpc::message(request_data));
        REQUIRE(request.method() == "test");
        REQUIRE(request.params_as<std::tuple<int, int, int>>() ==
            std::make_tuple(1, 2, 3));

        constexpr int result = 37;
        const auto response_data =
            mprpc::pack_response(request.msgid(), result);
        connector->add_read_data(response_data);

        const auto timeout = std::chrono::seconds(3);
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(future.get() == result);
    }

    SECTION("notify") {
        mprpc::client client{logger, threads, connector};
        client.start();

        REQUIRE_NOTHROW(client.notify("test", 1, 2, 3));

        const auto request_data = connector->get_written_data();
        mprpc::message request;
        REQUIRE_NOTHROW(request = mprpc::message(request_data));
        REQUIRE(request.method() == "test");
        REQUIRE(request.params_as<std::tuple<int, int, int>>() ==
            std::make_tuple(1, 2, 3));
    }

    SECTION("async_request for void result") {
        mprpc::client client{logger, threads, connector};
        client.start();

        std::future<void> future;
        REQUIRE_NOTHROW(
            future = client.async_request<void>("test", 1, 2, 3).get_future());

        const auto request_data = connector->get_written_data();
        mprpc::message request;
        REQUIRE_NOTHROW(request = mprpc::message(request_data));
        REQUIRE(request.method() == "test");
        REQUIRE(request.params_as<std::tuple<int, int, int>>() ==
            std::make_tuple(1, 2, 3));

        const auto response_data = mprpc::pack_response(request.msgid(), 0);
        connector->add_read_data(response_data);

        const auto timeout = std::chrono::seconds(3);
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        REQUIRE_NOTHROW(future.get());
    }
}
