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
 * \brief test of format_message function
 */
#include "mprpc/format_message.h"

#include <type_traits>

#include <catch2/catch.hpp>
#include <msgpack.hpp>

namespace {

std::string format_data(const std::string& data) {
    return mprpc::format_message(mprpc::message_data(data.data(), data.size()));
}

std::string format_data(const char* data, std::size_t size) {
    return mprpc::format_message(mprpc::message_data(data, size));
}

}  // namespace

TEST_CASE("mprpc::format_message") {
    SECTION("format nil") { REQUIRE(format_data({char(0xC0)}) == "nil"); }

    SECTION("format booleans") {
        REQUIRE(format_data({char(0xC2)}) == "false");
        REQUIRE(format_data({char(0xC3)}) == "true");
    }

    SECTION("foramt integers") {
        REQUIRE(format_data({char(0x0F)}) == "15");
        REQUIRE(format_data({char(0xFF)}) == "-1");
    }

    SECTION("format float32") {
        msgpack::sbuffer buffer;
        msgpack::pack(buffer, 1.0F);
        REQUIRE(format_data(buffer.data(), buffer.size()) == "1.00000F");
    }

    SECTION("format float64") {
        msgpack::sbuffer buffer;
        msgpack::pack(buffer, 1.0);
        REQUIRE(format_data(buffer.data(), buffer.size()) == "1.00000000000");
    }

    SECTION("format string") {
        REQUIRE(format_data({char(0xA3), 'a', 'b', 'c'}) == R"("abc")");
    }

    SECTION("format too long string") {
        constexpr std::size_t size = 100;
        REQUIRE(format_data(std::string({char(0xD9), char(size)}) +
                    std::string(size, 'a')) == " ...");
    }

    SECTION("format binary") {
        REQUIRE(format_data({char(0xC4), char(0x03), char(0x1F), char(0x2C),
                    char(0xA9)}) == "hex(1F2CA9)");
    }

    SECTION("format too long binary") {
        constexpr std::size_t size = 100;
        REQUIRE(format_data(std::string({char(0xC4), char(size)}) +
                    std::string(size, 'a')) == " ...");
    }

    SECTION("format ext") {
        REQUIRE(
            format_data({char(0xD4), char(0x03), char(0x1F)}) == "ext(031F)");
    }

    SECTION("format too long ext") {
        constexpr std::size_t size = 100;
        REQUIRE(format_data(std::string({char(0xC7), char(size)}) +
                    std::string(size + 1, 'a')) == " ...");
    }

    SECTION("format empty array") {
        REQUIRE(format_data({char(0x90)}) == "[]");
    }

    SECTION("format array with elements") {
        REQUIRE(format_data({char(0x92), char(0x01), char(0x02)}) == "[1,2]");
    }

    SECTION("format empty map") { REQUIRE(format_data({char(0x80)}) == "{}"); }

    SECTION("format map with pairs") {
        REQUIRE(format_data({char(0x82), char(0x01), char(0x02), char(0x03),
                    char(0x04)}) == "{1:2,3:4}");
    }

    SECTION("parse error") {
        REQUIRE(format_data({char(0xC1)}) == "invalid_data(C1)");
    }

    SECTION("insufficient bytes") {
        REQUIRE(format_data({char(0x91)}) == "invalid_data(91)");
    }

    SECTION("too long data") {
        constexpr std::size_t size = 100;
        REQUIRE(
            format_data(std::string({char(0xDC), char(0x00), char(size)}) +
                std::string(size, char(0x01)))
                .size() <= mprpc::impl::default_formatted_data_length_limit);
    }
}

TEST_CASE("mprpc::logging::log_data_traits<mprpc::message_data>") {
    SECTION("preprocess data") {
        const auto data_str = std::string({char(0x92), char(0x01), char(0x02)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());
        const auto str =
            mprpc::logging::log_data_traits<mprpc::message_data>::preprocess(
                data);
        REQUIRE(str == "[1,2]");
    }
}
