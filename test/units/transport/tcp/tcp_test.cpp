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
 * \brief test of TCP transport
 */
#include <future>
#include <thread>

#include <catch2/catch.hpp>

#include "../../create_logger.h"
#include "mprpc/transport/parsers/msgpack_parser.h"
#include "mprpc/transport/tcp/tcp_acceptor.h"
#include "mprpc/transport/tcp/tcp_connector.h"
#include "mprpc/transport/tcp/tcp_session.h"

TEST_CASE("mprpc::transport::tcp") {
    const auto logger = create_logger("mprpc::transport::tcp");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
    threads->start();

    const auto parser_factory =
        std::make_shared<mprpc::transport::parsers::msgpack_parser_factory>();

    // 127.0.0.1:3780
    constexpr std::uint16_t port = 3780;
    const auto endpoint =
        asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), port);

    SECTION("communicate") {
        auto acceptor = std::make_shared<mprpc::transport::tcp::tcp_acceptor>(
            logger, endpoint, threads->context(), parser_factory);
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

        auto connector_socket = asio::ip::tcp::socket(threads->context());
        REQUIRE_NOTHROW(connector_socket.connect(endpoint));
        auto connector = std::make_shared<mprpc::transport::tcp::tcp_connector>(
            logger, std::move(connector_socket), threads->context(),
            parser_factory->create_streaming_parser(logger));

        const auto timeout = std::chrono::seconds(15);
        REQUIRE(session_future.wait_for(timeout) == std::future_status::ready);
        std::shared_ptr<mprpc::transport::session> session;
        REQUIRE_NOTHROW(session = session_future.get());

        REQUIRE(connector->remote_address()->full_address() ==
            acceptor->local_address()->full_address());
        REQUIRE(connector->local_address()->full_address() ==
            session->remote_address()->full_address());

        SECTION("client to server transport") {
            MPRPC_INFO(logger, "client to server transport");
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

            auto result_promise = std::promise<void>();
            auto result_future = result_promise.get_future();
            const auto data_str =
                std::string({char(0x92), char(0x01), char(0x02)});
            const auto data =
                mprpc::message_data(data_str.data(), data_str.size());
            connector->async_write(
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

        SECTION("client to server transport twice") {
            MPRPC_INFO(logger, "client to server transport twice");
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

            const auto data_str1 = std::string({char(0x92), char(0x01)});
            const auto data1 =
                mprpc::message_data(data_str1.data(), data_str1.size());
            const auto data_str2 = std::string({char(0x02)});
            const auto data2 =
                mprpc::message_data(data_str2.data(), data_str2.size());
            auto empty_write_handler = [](const mprpc::error_info& error) {};
            connector->async_write(data1, empty_write_handler);
            connector->async_write(data2, empty_write_handler);

            const auto data_str =
                std::string({char(0x92), char(0x01), char(0x02)});
            const auto data =
                mprpc::message_data(data_str.data(), data_str.size());
            REQUIRE(
                message_future.wait_for(timeout) == std::future_status::ready);
            REQUIRE(message_future.get() == data);
        }

        SECTION("server to client transport with large data") {
            MPRPC_INFO(logger, "server to client transport with large data");
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
            const auto str = std::string(1024 * 1024, 'a');
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
