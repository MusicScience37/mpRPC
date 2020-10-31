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
 * \brief test of message_data class
 */
#include "mprpc/message_data.h"

#include <sstream>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "mprpc/format_message.h"

TEST_CASE("mprpc::message_data") {
    SECTION("default functions") {
        using test_type = mprpc::message_data;
        STATIC_REQUIRE(std::is_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("default construct") {
        const auto data = mprpc::message_data();
        REQUIRE(data.size() == 0);
    }

    SECTION("construct and get with pointer and size") {
        // NOLINTNEXTLINE: for test
        const char test_data[] = "a\0c";
        constexpr std::size_t test_data_size = sizeof(test_data) - 1;
        // NOLINTNEXTLINE: for test
        const auto data = mprpc::message_data(test_data, test_data_size);
        REQUIRE(data.size() == test_data_size);
        // NOLINTNEXTLINE: for test
        REQUIRE(data.data()[0] == test_data[0]);
        // NOLINTNEXTLINE: for test
        REQUIRE(data.data()[1] == test_data[1]);
        // NOLINTNEXTLINE: for test
        REQUIRE(data.data()[2] == test_data[2]);
    }

    SECTION("compare two messsages") {
        const auto data1_str =
            std::string({char(0x92), char(0x01), char(0x02)});
        const auto data1 =
            mprpc::message_data(data1_str.data(), data1_str.size());
        const auto data2 = data1;  // NOLINT: for test
        const auto data3_str =
            std::string({char(0x92), char(0x01), char(0x03)});
        const auto data3 =
            mprpc::message_data(data3_str.data(), data3_str.size());

        REQUIRE(data1 == data2);
        REQUIRE_FALSE(data1 == data3);
        REQUIRE_FALSE(data1 != data2);
        REQUIRE(data1 != data3);
    }

    SECTION("output data to a stream") {
        const auto data_str = std::string({char(0x92), char(0x01), char(0x02)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());

        std::ostringstream stream;
        stream << data;
        REQUIRE(stream.str() == "[1,2]");
    }
}
