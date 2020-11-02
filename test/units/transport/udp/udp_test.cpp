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
 * \brief test of UDP transport
 */
#include "mprpc/transport/udp/udp.h"

#include <future>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <msgpack.hpp>

#include "../../create_logger.h"
#include "mprpc/logging/logging_macros.h"
#include "mprpc/transport/parsers/msgpack_parser.h"

TEST_CASE("mprpc::transport::udp") {
    const auto logger = create_logger("mprpc::transport::udp");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
    threads->start();

    const auto parser_factory =
        std::make_shared<mprpc::transport::parsers::msgpack_parser_factory>();

    const auto host = std::string("127.0.0.1");
    constexpr std::uint16_t port = 3780;

    SECTION("communicate") {
        auto acceptor = mprpc::transport::udp::create_udp_acceptor(
            logger, host, port, *threads, parser_factory);
        auto session_promise =
            std::promise<std::shared_ptr<mprpc::transport::session>>();
        auto session_future = session_promise.get_future();
        acceptor->async_accept(
            [&session_promise](const mprpc::error_info& error,
                const std::shared_ptr<mprpc::transport::session>& session) {
                if (error) {
                    session_promise.set_exception(
                        std::make_exception_ptr(mprpc::exception(error)));
                } else {
                    session_promise.set_value(session);
                }
            });

        const auto wait_duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(wait_duration);

        auto connector = mprpc::transport::udp::create_udp_connector(
            logger, host, port, *threads, parser_factory);

        MPRPC_INFO(logger, "client to server transport");
        auto send_result_promise = std::promise<void>();
        auto send_result_future = send_result_promise.get_future();
        const auto data_str = std::string({char(0x92), char(0x01), char(0x02)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());
        connector->async_write(
            data, [&send_result_promise](const mprpc::error_info& error) {
                if (error) {
                    send_result_promise.set_exception(
                        std::make_exception_ptr(mprpc::exception(error)));
                } else {
                    send_result_promise.set_value();
                }
            });

        const auto timeout = std::chrono::seconds(15);
        REQUIRE(session_future.wait_for(timeout) == std::future_status::ready);
        std::shared_ptr<mprpc::transport::session> session;
        REQUIRE_NOTHROW(session = session_future.get());

        REQUIRE(connector->remote_address()->full_address() ==
            acceptor->local_address()->full_address());

        {
            auto message_promise = std::promise<mprpc::message_data>();
            auto message_future = message_promise.get_future();
            session->async_read(
                [&message_promise](const mprpc::error_info& error,
                    const mprpc::message_data& message) {
                    if (error) {
                        message_promise.set_exception(
                            std::make_exception_ptr(mprpc::exception(error)));
                    } else {
                        message_promise.set_value(message);
                    }
                });

            REQUIRE(
                message_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(send_result_future.wait_for(timeout) ==
                std::future_status::ready);
            REQUIRE(message_future.get() == data);
            REQUIRE_NOTHROW(send_result_future.get());
        }

        {
            MPRPC_INFO(logger, "server to client transport");
            auto message_promise = std::promise<mprpc::message_data>();
            auto message_future = message_promise.get_future();
            connector->async_read(
                [&message_promise](const mprpc::error_info& error,
                    const mprpc::message_data& message) {
                    if (error) {
                        message_promise.set_exception(
                            std::make_exception_ptr(mprpc::exception(error)));
                    } else {
                        message_promise.set_value(message);
                    }
                });

            auto result_promise = std::promise<void>();
            auto result_future = result_promise.get_future();
            const auto str = std::string(1024, 'a');
            msgpack::sbuffer buf;
            msgpack::pack(buf, str);
            const auto data = mprpc::message_data(buf.data(), buf.size());
            session->async_write(
                data, [&result_promise](const mprpc::error_info& error) {
                    if (error) {
                        result_promise.set_exception(
                            std::make_exception_ptr(mprpc::exception(error)));
                    } else {
                        result_promise.set_value();
                    }
                });

            REQUIRE(
                message_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(
                result_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(message_future.get() == data);
            REQUIRE_NOTHROW(result_future.get());
        }

        REQUIRE_NOTHROW(threads->stop());
    }
}
