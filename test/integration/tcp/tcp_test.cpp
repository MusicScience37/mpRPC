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
 * \brief test of RPC on TCP
 */
#include "mprpc/transport/tcp/tcp.h"

#include <future>

#include <catch2/catch.hpp>

#include "mprpc/execution/function_method_executor.h"
#include "mprpc/execution/simple_method_server.h"
#include "mprpc/logging/spdlog_logger.h"
#include "mprpc/server.h"
#include "mprpc/transport/parsers/msgpack_parser.h"

TEST_CASE("RPC on TCP") {
    constexpr std::size_t max_file_size = 1024 * 1024;
    constexpr std::size_t max_files = 5;
    auto logger = mprpc::logging::create_file_logger("mprpc_test_integ_tcp",
        "mprpc_test_integ_tcp.log", mprpc::logging::log_level::trace,
        max_file_size, max_files, true);

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
    threads->start();

    std::vector<std::shared_ptr<mprpc::execution::method_executor>> methods;

    methods.push_back(
        mprpc::execution::make_function_method_executor<std::string(
            std::string)>(logger, "echo", [](std::string str) { return str; }));

    const auto method_server =
        std::make_shared<mprpc::execution::simple_method_server>(
            logger, *threads, methods);

    const auto parser_factory =
        std::make_shared<mprpc::transport::parsers::msgpack_parser_factory>();

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    auto acceptor = mprpc::transport::tcp::create_tcp_acceptor(
        logger, host, port, *threads, parser_factory);

    auto server = mprpc::server(logger, threads, {acceptor}, method_server);
    server.start();

    const auto duration = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(duration);

    auto connector = mprpc::transport::tcp::create_tcp_connector(
        logger, host, port, *threads, parser_factory);

    SECTION("request") {
        constexpr mprpc::msgid_t msgid = 5;
        const auto str = std::string("abc");
        const auto message =
            mprpc::pack_request(msgid, "echo", std::make_tuple(str));
        {
            auto promise = std::promise<void>();
            auto future = promise.get_future();
            connector->async_write(
                message, [&promise](const mprpc::error_info& error) {
                    if (error) {
                        promise.set_exception(
                            std::make_exception_ptr(mprpc::exception(error)));
                    } else {
                        promise.set_value();
                    }
                });
            REQUIRE_NOTHROW(future.get());
        }
        mprpc::message response;
        {
            auto promise = std::promise<mprpc::message_data>();
            auto future = promise.get_future();
            connector->async_read([&promise](const mprpc::error_info& error,
                                      const mprpc::message_data& message) {
                if (error) {
                    promise.set_exception(
                        std::make_exception_ptr(mprpc::exception(error)));
                } else {
                    promise.set_value(message);
                }
            });
            REQUIRE_NOTHROW(response = mprpc::message(future.get()));
        }
        REQUIRE(response.type() == mprpc::msgtype::response);
        REQUIRE(response.msgid() == msgid);
        REQUIRE(response.has_error() == false);
        REQUIRE(response.result_as<std::string>() == str);
    }

    threads->stop();
}
