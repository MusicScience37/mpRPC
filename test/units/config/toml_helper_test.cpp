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
#include "mprpc/transport/compression_config.h"
#include "mprpc/transport/compressors/zstd_compressor_config.h"

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
