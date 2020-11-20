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

TEST_CASE("toml::from<mprpc::config::option>") {
    using test_type = mprpc::config::option<test_number>;

    SECTION("normal value") {
        constexpr std::uint16_t true_value = 37;
        const auto data = read_toml_str("value = 37");
        mprpc::config::option<test_number> option;
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
