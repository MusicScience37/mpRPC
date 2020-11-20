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
 * \brief test of option class
 */
#include "mprpc/config/toml_helper.h"

#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "catch2/matchers/catch_matchers.hpp"
#include "mprpc/client_config.h"
#include "mprpc/server.h"
#include "mprpc/transport/compression_config.h"
#include "mprpc/transport/compressors/zstd_compressor_config.h"
#include "mprpc/transport/tcp/tcp_acceptor_config.h"

namespace {

struct test_number {
    using value_type = std::uint16_t;

    static value_type default_value() noexcept { return 1; }

    static bool validate(value_type value) noexcept { return value != 0; }

    static std::string name() { return "test_config"; }

    static std::string description() { return "configuration for test"; }
};

toml::value read_toml_str(const std::string& str) {
    std::istringstream stream;
    stream.str(str);
    return toml::parse(stream, "inline_test_string.toml");
}

}  // namespace

TEST_CASE("toml::from<mprpc::config::option<test_number>>") {
    using test_type = mprpc::config::option<test_number>;

    SECTION("normal value") {
        constexpr std::uint16_t true_value = 37;
        const auto data = read_toml_str("value = 37");
        test_type option;
        REQUIRE_NOTHROW(option = toml::get<test_type>(data.at("value")));
        REQUIRE(option.value() == true_value);
    }

    SECTION("invalid value") {
        const auto data = read_toml_str("value = 0");
        mprpc::config::option<test_number> option;
        REQUIRE_THROWS_WITH(option = toml::get<test_type>(data.at("value")),
            Catch::Matchers::Contains(
                "invalid value for test_config, configuration for test"));
    }
}

TEST_CASE(
    "toml::from<mprpc::config::option<mprpc::transport::compression_type_type>"
    ">") {
    using test_type =
        mprpc::config::option<mprpc::transport::compression_type_type>;

    SECTION("none") {
        constexpr auto true_value = mprpc::transport::compression_type::none;
        const auto data = read_toml_str(R"(value = "none")");
        test_type option;
        REQUIRE_NOTHROW(option = toml::get<test_type>(data.at("value")));
        REQUIRE(option.value() == true_value);
    }

    SECTION("zstd") {
        constexpr auto true_value = mprpc::transport::compression_type::zstd;
        const auto data = read_toml_str(R"(value = "zstd")");
        test_type option;
        REQUIRE_NOTHROW(option = toml::get<test_type>(data.at("value")));
        REQUIRE(option.value() == true_value);
    }

    SECTION("invalid") {
        const auto data = read_toml_str(R"(value = "abc")");
        test_type option;
        REQUIRE_THROWS_WITH(option = toml::get<test_type>(data.at("value")),
            Catch::Matchers::Contains(
                "invalid value for type, compression type (none, zstd)"));
    }
}

TEST_CASE("toml::from<mprpc::transport::compression_config>") {
    using test_type = mprpc::transport::compression_config;

    SECTION("empty") {
        const auto data = read_toml_str(R"(
            [compression]
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("compression")));
        REQUIRE(
            config.type.value() == mprpc::transport::compression_type::none);
        REQUIRE(config.zstd_compression_level.value() ==
            mprpc::transport::compressors::zstd_compression_level_type::
                default_value());
    }

    SECTION("valid config") {
        const auto data = read_toml_str(R"(
            [compression]
            type = "zstd"
            zstd_compression_level = 5
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("compression")));
        REQUIRE(
            config.type.value() == mprpc::transport::compression_type::zstd);
        REQUIRE(config.zstd_compression_level.value() == 5);
    }

    SECTION("non-related key") {
        const auto data = read_toml_str(R"(
            [compression]
            non_related = 0
        )");
        test_type config;
        REQUIRE_THROWS_WITH(
            config = toml::get<test_type>(data.at("compression")),
            Catch::Matchers::Contains("invalid key non_related"));
    }
}

TEST_CASE("toml::from<mprpc::transport::tcp::tcp_acceptor_config>") {
    using test_type = mprpc::transport::tcp::tcp_acceptor_config;

    SECTION("empty") {
        const auto data = read_toml_str(R"(
            [tcp_acceptor]
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("tcp_acceptor")));
    }

    SECTION("valid config") {
        const auto data = read_toml_str(R"(
            [tcp_acceptor]
            host = "0.0.0.0"
            port = 1
            streaming_min_buf_size = 3
            compression.type = "zstd"
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("tcp_acceptor")));
        REQUIRE(config.host.value() == "0.0.0.0");
        REQUIRE(config.port.value() == 1);
        REQUIRE(config.streaming_min_buf_size.value() == 3);
        REQUIRE(config.compression.type.value() ==
            mprpc::transport::compression_type::zstd);
    }

    SECTION("non-related key") {
        const auto data = read_toml_str(R"(
            [tcp_acceptor]
            non_related = 0
        )");
        test_type config;
        REQUIRE_THROWS_WITH(
            config = toml::get<test_type>(data.at("tcp_acceptor")),
            Catch::Matchers::Contains("invalid key non_related"));
    }
}

TEST_CASE("toml::from<mprpc::transport::tcp::tcp_connector_config>") {
    using test_type = mprpc::transport::tcp::tcp_connector_config;

    SECTION("empty") {
        const auto data = read_toml_str(R"(
            [tcp_connector]
        )");
        test_type config;
        REQUIRE_NOTHROW(
            config = toml::get<test_type>(data.at("tcp_connector")));
    }

    SECTION("valid config") {
        const auto data = read_toml_str(R"(
            [tcp_connector]
            host = "0.0.0.0"
            port = 1
            streaming_min_buf_size = 3
            compression.type = "zstd"
        )");
        test_type config;
        REQUIRE_NOTHROW(
            config = toml::get<test_type>(data.at("tcp_connector")));
        REQUIRE(config.host.value() == "0.0.0.0");
        REQUIRE(config.port.value() == 1);
        REQUIRE(config.streaming_min_buf_size.value() == 3);
        REQUIRE(config.compression.type.value() ==
            mprpc::transport::compression_type::zstd);
    }

    SECTION("non-related key") {
        const auto data = read_toml_str(R"(
            [tcp_connector]
            non_related = 0
        )");
        test_type config;
        REQUIRE_THROWS_WITH(
            config = toml::get<test_type>(data.at("tcp_connector")),
            Catch::Matchers::Contains("invalid key non_related"));
    }
}

TEST_CASE("toml::from<mprpc::transport::udp::udp_acceptor_config>") {
    using test_type = mprpc::transport::udp::udp_acceptor_config;

    SECTION("empty") {
        const auto data = read_toml_str(R"(
            [udp_acceptor]
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("udp_acceptor")));
    }

    SECTION("valid config") {
        const auto data = read_toml_str(R"(
            [udp_acceptor]
            host = "0.0.0.0"
            port = 1
            datagram_buf_size = 3
            compression.type = "zstd"
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("udp_acceptor")));
        REQUIRE(config.host.value() == "0.0.0.0");
        REQUIRE(config.port.value() == 1);
        REQUIRE(config.datagram_buf_size.value() == 3);
        REQUIRE(config.compression.type.value() ==
            mprpc::transport::compression_type::zstd);
    }

    SECTION("non-related key") {
        const auto data = read_toml_str(R"(
            [udp_acceptor]
            non_related = 0
        )");
        test_type config;
        REQUIRE_THROWS_WITH(
            config = toml::get<test_type>(data.at("udp_acceptor")),
            Catch::Matchers::Contains("invalid key non_related"));
    }
}

TEST_CASE("toml::from<mprpc::transport::udp::udp_connector_config>") {
    using test_type = mprpc::transport::udp::udp_connector_config;

    SECTION("empty") {
        const auto data = read_toml_str(R"(
            [udp_connector]
        )");
        test_type config;
        REQUIRE_NOTHROW(
            config = toml::get<test_type>(data.at("udp_connector")));
    }

    SECTION("valid config") {
        const auto data = read_toml_str(R"(
            [udp_connector]
            host = "0.0.0.0"
            port = 1
            datagram_buf_size = 3
            compression.type = "zstd"
        )");
        test_type config;
        REQUIRE_NOTHROW(
            config = toml::get<test_type>(data.at("udp_connector")));
        REQUIRE(config.host.value() == "0.0.0.0");
        REQUIRE(config.port.value() == 1);
        REQUIRE(config.datagram_buf_size.value() == 3);
        REQUIRE(config.compression.type.value() ==
            mprpc::transport::compression_type::zstd);
    }

    SECTION("non-related key") {
        const auto data = read_toml_str(R"(
            [udp_connector]
            non_related = 0
        )");
        test_type config;
        REQUIRE_THROWS_WITH(
            config = toml::get<test_type>(data.at("udp_connector")),
            Catch::Matchers::Contains("invalid key non_related"));
    }
}

TEST_CASE("toml::from<mprpc::server_config>") {
    using test_type = mprpc::server_config;

    SECTION("empty") {
        const auto data = read_toml_str(R"(
            [server]
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("server")));
    }

    SECTION("valid config") {
        const auto data = read_toml_str(R"(
            server.num_threads = 2
            [[server.tcp_acceptors]]
            host = "0.0.0.0"
            [[server.udp_acceptors]]
            host = "0.0.0.1"
            [[server.udp_acceptors]]
            host = "0.0.0.2"
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("server")));
        REQUIRE(config.num_threads.value() == 2);
        REQUIRE(config.tcp_acceptors.size() == 1);
        REQUIRE(config.tcp_acceptors.at(0).host.value() == "0.0.0.0");
        REQUIRE(config.udp_acceptors.size() == 2);
        REQUIRE(config.udp_acceptors.at(0).host.value() == "0.0.0.1");
        REQUIRE(config.udp_acceptors.at(1).host.value() == "0.0.0.2");
    }

    SECTION("non-related key") {
        const auto data = read_toml_str(R"(
            [server]
            non_related = 0
        )");
        test_type config;
        REQUIRE_THROWS_WITH(config = toml::get<test_type>(data.at("server")),
            Catch::Matchers::Contains("invalid key non_related"));
    }
}

TEST_CASE(
    "toml::from<mprpc::config::option<mprpc::config::option<mprpc::connector_"
    "type_type>>") {
    using test_type = mprpc::config::option<mprpc::connector_type_type>;

    SECTION("tcp") {
        constexpr auto true_value = mprpc::transport_type::tcp;
        const auto data = read_toml_str(R"(value = "tcp")");
        test_type option;
        REQUIRE_NOTHROW(option = toml::get<test_type>(data.at("value")));
        REQUIRE(option.value() == true_value);
    }

    SECTION("TCP") {
        constexpr auto true_value = mprpc::transport_type::tcp;
        const auto data = read_toml_str(R"(value = "TCP")");
        test_type option;
        REQUIRE_NOTHROW(option = toml::get<test_type>(data.at("value")));
        REQUIRE(option.value() == true_value);
    }

    SECTION("udp") {
        constexpr auto true_value = mprpc::transport_type::udp;
        const auto data = read_toml_str(R"(value = "udp")");
        test_type option;
        REQUIRE_NOTHROW(option = toml::get<test_type>(data.at("value")));
        REQUIRE(option.value() == true_value);
    }

    SECTION("UDP") {
        constexpr auto true_value = mprpc::transport_type::udp;
        const auto data = read_toml_str(R"(value = "UDP")");
        test_type option;
        REQUIRE_NOTHROW(option = toml::get<test_type>(data.at("value")));
        REQUIRE(option.value() == true_value);
    }

    SECTION("invalid") {
        const auto data = read_toml_str(R"(value = "abc")");
        test_type option;
        REQUIRE_THROWS_WITH(option = toml::get<test_type>(data.at("value")),
            Catch::Matchers::Contains(
                "invalid value for transport type (TCP, UDP)"));
    }
}

TEST_CASE("toml::from<mprpc::client_config>") {
    using test_type = mprpc::client_config;

    SECTION("empty") {
        const auto data = read_toml_str(R"(
            [client]
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("client")));
    }

    SECTION("valid config") {
        const auto data = read_toml_str(R"(
            [client]
            num_threads = 3
            sync_request_timeout_ms = 5
            connector_type = "UDP"
            [client.tcp_connector]
            host = "tcp_server"
            [client.udp_connector]
            host = "udp_server"
        )");
        test_type config;
        REQUIRE_NOTHROW(config = toml::get<test_type>(data.at("client")));
        REQUIRE(config.num_threads.value() == 3);
        REQUIRE(config.sync_request_timeout_ms.value() == 5);
        REQUIRE(config.connector_type.value() == mprpc::transport_type::udp);
        REQUIRE(config.tcp_connector.host.value() == "tcp_server");
        REQUIRE(config.udp_connector.host.value() == "udp_server");
    }

    SECTION("non-related key") {
        const auto data = read_toml_str(R"(
            [client]
            non_related = 0
        )");
        test_type config;
        REQUIRE_THROWS_WITH(config = toml::get<test_type>(data.at("client")),
            Catch::Matchers::Contains("invalid key non_related"));
    }
}
