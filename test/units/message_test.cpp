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
 * \brief test of message class
 */
#include "mprpc/message.h"

#include <type_traits>

#include <catch2/catch.hpp>

TEST_CASE("mprpc::message") {
    SECTION("default functions") {
        using test_type = mprpc::message;
        STATIC_REQUIRE(std::is_default_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_constructible<test_type>::value);
        STATIC_REQUIRE(std::is_nothrow_move_assignable<test_type>::value);
    }

    SECTION("parse request") {
        constexpr auto type = mprpc::msgtype::request;
        constexpr auto msgid = static_cast<mprpc::msgid_t>(37);
        const auto method = std::string("abc");
        const auto params = std::make_tuple(std::string("def"));
        const auto data =
            mprpc::pack_data(std::make_tuple(type, msgid, method, params));

        mprpc::message msg;
        REQUIRE_NOTHROW(msg = mprpc::message(data));
        REQUIRE(msg.type() == type);
        REQUIRE(msg.msgid() == msgid);
        REQUIRE(msg.method() == method);
        REQUIRE(msg.params_as<std::tuple<std::string>>() == params);
    }

    SECTION("parse response") {
        constexpr auto type = mprpc::msgtype::response;
        constexpr auto msgid = static_cast<mprpc::msgid_t>(37);
        const auto result = std::string("abc");
        const auto data = mprpc::pack_data(
            std::make_tuple(type, msgid, msgpack::type::nil_t(), result));

        mprpc::message msg;
        REQUIRE_NOTHROW(msg = mprpc::message(data));
        REQUIRE(msg.type() == type);
        REQUIRE(msg.msgid() == msgid);
        REQUIRE(msg.has_error() == false);
        REQUIRE(msg.error().is_nil());
        REQUIRE(msg.result_as<std::string>() == result);
    }

    SECTION("parse response with error") {
        constexpr auto type = mprpc::msgtype::response;
        constexpr auto msgid = static_cast<mprpc::msgid_t>(37);
        const auto error = std::string("abc");
        const auto data = mprpc::pack_data(
            std::make_tuple(type, msgid, error, msgpack::type::nil_t()));

        mprpc::message msg;
        REQUIRE_NOTHROW(msg = mprpc::message(data));
        REQUIRE(msg.type() == type);
        REQUIRE(msg.msgid() == msgid);
        REQUIRE(msg.has_error());
        REQUIRE(msg.error_as<std::string>() == error);
    }

    SECTION("parse request") {
        constexpr auto type = mprpc::msgtype::notification;
        const auto method = std::string("abc");
        const auto params = std::make_tuple(std::string("def"));
        const auto data =
            mprpc::pack_data(std::make_tuple(type, method, params));

        mprpc::message msg;
        REQUIRE_NOTHROW(msg = mprpc::message(data));
        REQUIRE(msg.type() == type);
        REQUIRE(msg.method() == method);
        REQUIRE(msg.params_as<std::tuple<std::string>>() == params);
    }
}
