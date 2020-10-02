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
 * \brief test of msgpack_parser class
 */
#include "mprpc/transport/msgpack/msgpack_parser.h"

#include <string>

#include <catch2/catch.hpp>

#include "../../create_logger.h"

TEST_CASE("mprpc::transport::msgpack::msgpack_parser") {
    auto logger = create_logger("mprpc::transport::msgpack::msgpack_parser");

    using mprpc::transport::msgpack::msgpack_parser;
    msgpack_parser parser(logger);

    SECTION("parse a message") {
        const auto data_str = std::string({char(0x92), char(0x01), char(0x02)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());

        mprpc::message_data parsed_data("", 0);
        REQUIRE_NOTHROW(parsed_data = parser.parse(data.data(), data.size()));
        REQUIRE(parsed_data == data);
    }

    SECTION("parse error") {
        const auto data_str = std::string({char(0xC1)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());
        REQUIRE_THROWS_AS(
            parser.parse(data.data(), data.size()), mprpc::exception);
    }

    SECTION("insufficient bytes") {
        const auto data_str = std::string({char(0x91)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());
        REQUIRE_THROWS_AS(
            parser.parse(data.data(), data.size()), mprpc::exception);
    }
}
