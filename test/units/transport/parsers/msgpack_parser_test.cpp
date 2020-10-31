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
#include "mprpc/transport/parsers/msgpack_parser.h"

#include <string>

#include <catch2/catch.hpp>

#include "../../create_logger.h"

TEST_CASE("mprpc::transport::parsers::msgpack_parser") {
    auto logger = create_logger("mprpc::transport::parsers::msgpack_parser");

    const auto parser_ptr =
        mprpc::transport::parsers::create_msgpack_parser(logger);
    auto& parser = *parser_ptr;

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

TEST_CASE("mprpc::transport::parsers::msgpack_streaming_parser") {
    auto logger =
        create_logger("mprpc::transport::parsers::msgpack_streaming_parser");

    const auto parser_ptr =
        mprpc::transport::parsers::create_msgpack_streaming_parser(logger);
    auto& parser = *parser_ptr;

    SECTION("parse a message") {
        const auto data_str = std::string({char(0x92), char(0x01), char(0x02)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());

        parser.prepare_buffer(data.size() + 2);
        std::copy(data.data(), data.data() + data.size(), parser.buffer());
        REQUIRE(parser.parse_next(data.size() + 1));
        REQUIRE(parser.get() == data);
    }

    SECTION("parse two messages") {
        auto data_str =  // NOLINTNEXTLINE: for test
            std::string({char(0x92), char(0x01), char(0x02), char(0x92)});
        auto data = mprpc::message_data(data_str.data(), data_str.size());
        parser.prepare_buffer(data.size() + 2);
        std::copy(data.data(), data.data() + data.size(), parser.buffer());
        REQUIRE(parser.parse_next(data.size()));

        // NOLINTNEXTLINE: for test
        data_str = std::string({char(0x92), char(0x01), char(0x02)});
        data = mprpc::message_data(data_str.data(), data_str.size());
        REQUIRE(parser.get() == data);

        // NOLINTNEXTLINE: for test
        data_str = std::string({char(0x03), char(0x04)});
        data = mprpc::message_data(data_str.data(), data_str.size());
        parser.prepare_buffer(data.size());
        std::copy(data.data(), data.data() + data.size(), parser.buffer());
        REQUIRE(parser.parse_next(data.size()));

        // NOLINTNEXTLINE: for test
        data_str = std::string({char(0x92), char(0x03), char(0x04)});
        data = mprpc::message_data(data_str.data(), data_str.size());
        REQUIRE(parser.get() == data);
    }

    SECTION("parse a message using consumed()") {
        const auto data_str = std::string({char(0x92), char(0x01), char(0x02)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());

        parser.prepare_buffer(data.size() + 2);
        std::copy(data.data(), data.data() + data.size(), parser.buffer());
        REQUIRE_NOTHROW(parser.consumed(data.size() + 1));
        REQUIRE(parser.parse_next(0));
        REQUIRE(parser.get() == data);
    }

    SECTION("parse error") {
        const auto data_str = std::string({char(0xC1)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());

        parser.prepare_buffer(data.size() + 2);
        std::copy(data.data(), data.data() + data.size(), parser.buffer());
        REQUIRE_THROWS_AS(parser.parse_next(data.size() + 1), mprpc::exception);
    }

    SECTION("insufficient bytes") {
        const auto data_str = std::string({char(0x92), char(0x01)});
        const auto data = mprpc::message_data(data_str.data(), data_str.size());

        parser.prepare_buffer(data.size() + 2);
        std::copy(data.data(), data.data() + data.size(), parser.buffer());
        REQUIRE_FALSE(parser.parse_next(data.size()));
    }
}
