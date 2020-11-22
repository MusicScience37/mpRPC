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
 * \brief test of simple_method_server class
 */
#include "mprpc/execution/simple_method_server.h"

#include <future>

#include <catch2/catch_test_macros.hpp>

#include "../create_logger.h"
#include "mprpc/execution/function_method_executor.h"
#include "mprpc/transport/mock/mock_session.h"

TEST_CASE("mprpc::execution::simple_method_server") {
    auto logger = create_logger("mprpc::execution::simple_method_server");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
    threads->start();

    std::vector<std::shared_ptr<mprpc::execution::method_executor>> methods;

    methods.push_back(mprpc::execution::make_function_method_executor<int(int)>(
        logger, "copy_int", [](int x) { return x; }));

    const auto server =
        std::make_shared<mprpc::execution::simple_method_server>(
            logger, *threads, methods);

    const auto session = std::make_shared<mprpc::transport::mock::mock_session>(
        threads->context());

    SECTION("request") {
        constexpr mprpc::msgid_t msgid = 5;
        constexpr int integer = 7;
        const auto message = mprpc::message(
            mprpc::pack_request(msgid, "copy_int", std::make_tuple(integer)));

        auto promise = std::promise<std::shared_ptr<mprpc::message_data>>();
        auto future = promise.get_future();

        server->async_process_message(session, message,
            [&promise](const mprpc::error_info& error, bool response_exists,
                const mprpc::message_data& message) {
                if (error) {
                    promise.set_exception(
                        std::make_exception_ptr(mprpc::exception(error)));
                } else if (response_exists) {
                    promise.set_value(
                        std::make_shared<mprpc::message_data>(message));
                } else {
                    promise.set_value(nullptr);
                }
            });

        const auto timeout = std::chrono::seconds(1);
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        std::shared_ptr<mprpc::message_data> response_data;
        REQUIRE_NOTHROW(response_data = future.get());
        REQUIRE(response_data);
        mprpc::message response_message;
        REQUIRE_NOTHROW(response_message = mprpc::message(*response_data));
        REQUIRE(response_message.type() == mprpc::msgtype::response);
        REQUIRE(response_message.msgid() == msgid);
        REQUIRE(response_message.has_error() == false);
        REQUIRE(response_message.result_as<int>() == integer);
    }

    SECTION("notification") {
        constexpr int integer = 7;
        const auto message = mprpc::message(
            mprpc::pack_notification("copy_int", std::make_tuple(integer)));

        auto promise = std::promise<std::shared_ptr<mprpc::message_data>>();
        auto future = promise.get_future();

        server->async_process_message(session, message,
            [&promise](const mprpc::error_info& error, bool response_exists,
                const mprpc::message_data& message) {
                if (error) {
                    promise.set_exception(
                        std::make_exception_ptr(mprpc::exception(error)));
                } else if (response_exists) {
                    promise.set_value(
                        std::make_shared<mprpc::message_data>(message));
                } else {
                    promise.set_value(nullptr);
                }
            });

        const auto timeout = std::chrono::seconds(1);
        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        std::shared_ptr<mprpc::message_data> response_data;
        REQUIRE_NOTHROW(response_data = future.get());
        REQUIRE(!response_data);
    }
}
