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
 * \brief test of loading configurations
 */
#include <catch2/catch_test_macros.hpp>

#include "mprpc/client_builder.h"
#include "mprpc/config/load.h"
#include "mprpc/logging/basic_loggers.h"
#include "mprpc/method_client.h"
#include "mprpc/server_builder.h"

TEST_CASE("load configuration for RPC") {
    static constexpr std::size_t max_file_size = 1024 * 1024;
    static constexpr std::size_t max_files = 5;
    static const auto logger =
        mprpc::logging::create_file_logger("mprpc_test_integ_config.log",
            mprpc::logging::log_level::trace, max_file_size, max_files, true);

    SECTION("load configuration and perform RPC") {
        const auto config = mprpc::config::load("mprpc_test_integ_config.toml");

        auto server = mprpc::server_builder(logger)
                          .server_config(config.server)
                          .method<std::string(std::string)>(
                              "echo", [](std::string str) { return str; })
                          .create();

        const auto duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(duration);

        auto client =
            mprpc::client_builder(logger).client_config(config.client).create();

        auto echo_client =
            mprpc::method_client<std::string(std::string)>(*client, "echo");

        const auto str = std::string("abc");
        REQUIRE(echo_client(str) == str);

        client->stop();
        server->stop();
        std::this_thread::sleep_for(duration);
    }
}
