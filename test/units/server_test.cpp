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
 * \brief test of server class
 */
#include "mprpc/server.h"

#include <future>

#include <catch2/catch.hpp>

#include "create_logger.h"
#include "mprpc/execution/function_method_executor.h"
#include "mprpc/execution/simple_method_server.h"
#include "mprpc/transport/mock/mock_acceptor.h"

TEST_CASE("mprpc::server") {
    const auto logger = create_logger("mprpc::server");

    const auto threads = std::make_shared<mprpc::thread_pool>(logger, 2);
    threads->start();

    std::vector<std::shared_ptr<mprpc::execution::method_executor>> methods;

    methods.push_back(mprpc::execution::make_function_method_executor<int(int)>(
        logger, "copy_int", [](int x) { return x; }));

    const auto method_server =
        std::make_shared<mprpc::execution::simple_method_server>(
            logger, *threads, methods);

    const auto acceptor =
        std::make_shared<mprpc::transport::mock::mock_acceptor>(
            threads->context());

    SECTION("request") {
        mprpc::server server{logger, threads, {acceptor}, method_server};
        server.start();

        const auto duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(duration);

        const auto session = acceptor->create_session();

        std::this_thread::sleep_for(duration);

        constexpr mprpc::msgid_t msgid = 5;
        constexpr int integer = 7;
        const auto message =
            mprpc::pack_request(msgid, "copy_int", std::make_tuple(integer));
        session->add_read_data(message);

        auto future = std::async([session, duration] {
            constexpr std::size_t repetitions = 100;
            for (std::size_t i = 0; i < repetitions; ++i) {
                auto data = session->get_written_data();
                if (data.size() > 0) {
                    return data;
                }
                std::this_thread::sleep_for(duration);
            }
            return mprpc::message_data();
        });
        mprpc::message response;
        REQUIRE_NOTHROW(response = mprpc::message(future.get()));
        REQUIRE(response.type() == mprpc::msgtype::response);
        REQUIRE(response.msgid() == msgid);
        REQUIRE(response.has_error() == false);
        REQUIRE(response.result_as<int>() == integer);
    }

    threads->stop();
}
