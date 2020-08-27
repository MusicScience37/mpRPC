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
 * \brief test of error_info class
 */
#include "mprpc/error_info.h"

#include <type_traits>

#include <catch2/catch.hpp>

TEST_CASE("mprpc::error_info") {
    SECTION("default functions") {
        using test_type = mprpc::error_info;
        STATIC_REQUIRE(std::is_nothrow_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("construct without error") {
        const auto info = mprpc::error_info();
        REQUIRE(info.has_error() == false);
        REQUIRE(info.operator bool() == false);
        REQUIRE(info.code() == mprpc::error_code::success);
        REQUIRE(info.message() == "");  // NOLINT
        REQUIRE(info.has_data() == false);
        REQUIRE(info.data().str() == "");  // NOLINT
    }

    SECTION("construct with error without data") {
        constexpr auto code = mprpc::error_code::unexpected_error;
        const auto message = std::string("abc");
        const auto info = mprpc::error_info(code, message);
        REQUIRE(info.has_error() == true);
        REQUIRE(info.operator bool() == true);
        REQUIRE(info.code() == code);
        REQUIRE(info.message() == message);
        REQUIRE(info.has_data() == false);
        REQUIRE(info.data().str() == "");  // NOLINT
    }

    SECTION("construct with error without data") {
        constexpr auto code = mprpc::error_code::unexpected_error;
        const auto message = std::string("abc");
        const auto data = mprpc::message_data("abc");
        const auto info = mprpc::error_info(code, message, data);
        REQUIRE(info.has_error() == true);
        REQUIRE(info.operator bool() == true);
        REQUIRE(info.code() == code);
        REQUIRE(info.message() == message);
        REQUIRE(info.has_data() == true);
        REQUIRE(info.data().str() == data.str());
    }
}
