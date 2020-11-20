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
 * \brief test of load function
 */
#include "mprpc/config/load.h"

#include <fstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "catch2/matchers/catch_matchers.hpp"
#include "mprpc/client_config.h"
#include "mprpc/error_code.h"
#include "mprpc/transport/compression_config.h"

namespace {

void write(const std::string& filepath, const std::string& contents) {
    std::ofstream stream(filepath);
    INFO("filepath = " << filepath);
    REQUIRE(stream);
    stream.write(contents.data(), contents.size());
    stream.flush();
    REQUIRE(stream);
}

}  // namespace

TEST_CASE("mprpc::config::load") {
    SECTION("valid configuration") {
        const auto* const filepath = "test_mprpc_config_load_valid.toml";
        write(filepath, R"(
            [client]
            num_threads = 3
            [client.tcp_connector]
            host = "localhost"
            port = 12345
            compression.type = "zstd"

            [server]
            [[server.tcp_acceptors]]
            host = "0.0.0.0"
            port = 12345
            compression.type = "zstd"
        )");

        constexpr std::uint16_t port = 12345;

        mprpc::mprpc_config config;
        REQUIRE_NOTHROW(config = mprpc::config::load(filepath));

        REQUIRE(config.client.num_threads.value() == 3);
        REQUIRE(
            config.client.connector_type.value() == mprpc::transport_type::tcp);
        REQUIRE(config.client.tcp_connector.host.value() == "localhost");
        REQUIRE(config.client.tcp_connector.port.value() == port);
        REQUIRE(config.client.tcp_connector.compression.type.value() ==
            mprpc::transport::compression_type::zstd);

        REQUIRE(config.server.tcp_acceptors.size() == 1);
        REQUIRE(config.server.tcp_acceptors.at(0).host.value() == "0.0.0.0");
        REQUIRE(config.server.tcp_acceptors.at(0).port.value() == port);
        REQUIRE(config.server.tcp_acceptors.at(0).compression.type.value() ==
            mprpc::transport::compression_type::zstd);
    }

    SECTION("invalid configuration") {
        const auto* const filepath = "test_mprpc_config_load_invalid.toml";
        write(filepath, R"(
            [client]
            num_threads = 3
            [client.tcp_connectors]
            host = "localhost"
        )");

        mprpc::mprpc_config config;
        try {
            config = mprpc::config::load(filepath);
            FAIL();
        } catch (const mprpc::exception& e) {
            REQUIRE(e.info().code() == mprpc::error_code::config_parse_error);
            REQUIRE_THAT(std::string(e.what()),
                Catch::Matchers::Contains("invalid key tcp_connectors"));
        }
    }
}
