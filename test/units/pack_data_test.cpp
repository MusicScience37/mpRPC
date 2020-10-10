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
 * \brief test of pack_data function
 */
#include "mprpc/pack_data.h"

#include <catch2/catch.hpp>

namespace {

msgpack::object_handle unpack(const mprpc::message_data& data) {
    return msgpack::unpack(data.data(), data.size());
}

}  // namespace

TEST_CASE("mprpc::pack_data") {
    SECTION("pack int") {
        constexpr int integer = 37;
        const auto data = mprpc::pack_data(integer);
        REQUIRE(unpack(data)->as<int>() == integer);
    }

    SECTION("pack msgpack::object") {
        constexpr int integer = 37;
        auto zone = msgpack::zone();
        const auto object = msgpack::object(integer, zone);
        const auto data = mprpc::pack_data(object);
        REQUIRE(unpack(data)->as<int>() == integer);
    }
}

TEST_CASE("mprpc::pack_request") {
    SECTION("pack a request data") {
        constexpr auto type = mprpc::msgtype::request;
        constexpr auto msgid = static_cast<mprpc::msgid_t>(37);
        const auto method = std::string("abc");
        const auto params = std::make_tuple(std::string("def"));
        const auto data = mprpc::pack_request(msgid, method, params);

        msgpack::object_handle handle;
        REQUIRE_NOTHROW(handle = msgpack::unpack(data.data(), data.size()));
        std::tuple<mprpc::msgtype, mprpc::msgid_t, std::string,
            std::tuple<std::string>>
            parsed_data;
        REQUIRE_NOTHROW(parsed_data = handle->as<decltype(parsed_data)>());
        REQUIRE(std::get<0>(parsed_data) == type);
        REQUIRE(std::get<1>(parsed_data) == msgid);
        REQUIRE(std::get<2>(parsed_data) == method);
        REQUIRE(std::get<3>(parsed_data) == params);
    }
}

TEST_CASE("mprpc::pack_response") {
    SECTION("pack a response data") {
        constexpr auto type = mprpc::msgtype::response;
        constexpr auto msgid = static_cast<mprpc::msgid_t>(37);
        const auto result = std::string("abc");
        const auto data = mprpc::pack_response(msgid, result);

        msgpack::object_handle handle;
        REQUIRE_NOTHROW(handle = msgpack::unpack(data.data(), data.size()));
        std::tuple<mprpc::msgtype, mprpc::msgid_t, msgpack::type::nil_t,
            std::string>
            parsed_data;
        REQUIRE_NOTHROW(parsed_data = handle->as<decltype(parsed_data)>());
        REQUIRE(std::get<0>(parsed_data) == type);
        REQUIRE(std::get<1>(parsed_data) == msgid);
        REQUIRE(std::get<3>(parsed_data) == result);
    }
}

TEST_CASE("mprpc::pack_error_response") {
    SECTION("pack a response data") {
        constexpr auto type = mprpc::msgtype::response;
        constexpr auto msgid = static_cast<mprpc::msgid_t>(37);
        const auto error = std::string("abc");
        const auto data = mprpc::pack_error_response(msgid, error);

        msgpack::object_handle handle;
        REQUIRE_NOTHROW(handle = msgpack::unpack(data.data(), data.size()));
        std::tuple<mprpc::msgtype, mprpc::msgid_t, std::string,
            msgpack::type::nil_t>
            parsed_data;
        REQUIRE_NOTHROW(parsed_data = handle->as<decltype(parsed_data)>());
        REQUIRE(std::get<0>(parsed_data) == type);
        REQUIRE(std::get<1>(parsed_data) == msgid);
        REQUIRE(std::get<2>(parsed_data) == error);
    }
}
