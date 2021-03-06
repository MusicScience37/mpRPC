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
 * \brief test of exception class
 */
#include "mprpc/exception.h"

#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("mprpc::exception") {
    SECTION("default functions") {
        using test_type = mprpc::exception;
        STATIC_REQUIRE_FALSE(std::is_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("throw an exception") {
        static constexpr auto code = mprpc::error_code::unexpected_error;
        const auto message = std::string("abc");
        const auto data_str = std::string("abc");
        const auto data = mprpc::message_data(data_str.data(), data_str.size());
        auto func = [&message, &data] {
            throw mprpc::exception(mprpc::error_info(code, message, data));
        };

        try {
            func();
        } catch (const mprpc::exception& exception) {
            const auto& info = exception.info();
            REQUIRE(info.has_error() == true);
            REQUIRE(info.operator bool() == true);
            REQUIRE(info.code() == code);
            REQUIRE(info.message() == message);
            REQUIRE(info.has_data() == true);
            REQUIRE(std::string(data.data(), data.size()) == data_str);
        }

        REQUIRE_THROWS_AS(func(), mprpc::exception);
        REQUIRE_THROWS_WITH(func(), "abc");
    }
}
