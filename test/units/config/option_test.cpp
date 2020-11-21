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
#include "mprpc/config/option.h"

#include <cstdint>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

struct test_number {
    using value_type = std::uint16_t;

    static value_type default_value() noexcept { return 1; }

    static bool validate(value_type value) noexcept { return value != 0; }

    static std::string name() { return "test_config"; }

    static std::string description() { return "configuration for test"; }
};

}  // namespace

TEST_CASE("mprpc::config::option") {
    SECTION("option of number") {
        using test_type = mprpc::config::option<test_number>;

        STATIC_REQUIRE(std::is_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_move_assignable<test_type>::value);

        STATIC_REQUIRE(
            std::is_same<typename test_type::value_type, std::uint16_t>::value);

        test_type opt;
        constexpr std::uint16_t value = 37;
        REQUIRE_NOTHROW(opt = value);
        REQUIRE(opt.value() == value);
        REQUIRE_THROWS_WITH(
            opt = 0, "invalid value for test_config, configuration for test");
    }
}
