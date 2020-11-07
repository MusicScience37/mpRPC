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
 * \brief test of server_builder class
 */
#include "mprpc/server_builder.h"

#include <future>
#include <tuple>

#include <catch2/catch_test_macros.hpp>

#include "create_logger.h"
#include "mprpc/pack_data.h"

TEST_CASE("mprpc::server_builder") {
    const auto logger = create_logger("mprpc::server_builder");

    SECTION("create a server") {
        const auto host = std::string("127.0.0.1");
        constexpr std::uint16_t port = 3780;

        auto server = mprpc::server_builder(logger)
                          .num_threads(2)
                          .listen_tcp(host, port)
                          .method<std::string(std::string)>(
                              "echo", [](std::string str) { return str; })
                          .create();

        const auto wait_duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(wait_duration);

        const auto threads = std::make_shared<mprpc::thread_pool>(logger, 3);
        threads->start();

        const auto comp_factory = std::make_shared<
            mprpc::transport::compressors::null_compressor_factory>();

        const auto parser_factory = std::make_shared<
            mprpc::transport::parsers::msgpack_parser_factory>();

        const auto connector = mprpc::transport::tcp::create_tcp_connector(
            logger, host, port, *threads, comp_factory, parser_factory);

        const mprpc::msgid_t msgid = 37;
        const auto str = std::string("abcde");
        const auto request =
            mprpc::pack_request(msgid, "echo", std::make_tuple(str));
        {
            auto promise = std::promise<void>();
            auto future = promise.get_future();
            connector->async_write(
                request, [&promise](const mprpc::error_info& error) {
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

        threads->stop();
    }
}
